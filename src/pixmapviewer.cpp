#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include "pixmapviewer.h"

PixmapViewer::PixmapViewer():QWidget()
{
	setBackgroundRole(QPalette::Dark);
//	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
// 	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	m_zoomFactor = 1;
	m_bestFit = true;
	m_userImage = QImage();
}

PixmapViewer::PixmapViewer( QWidget *parent ):QWidget(parent)
{
	setBackgroundRole(QPalette::Base);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
 	//setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	m_bestFit = true;
	m_zoomFactor = 1;
}

PixmapViewer::~PixmapViewer()
{
}

void PixmapViewer::loadImage( QString f )
{
	m_userImage = QImage( f );
	zoomBy( 1 );
}

void PixmapViewer::setPixmap( QPixmap p )
{
	m_userImage = p.toImage();
	zoomBy( 1 );
}

void PixmapViewer::setImage( QImage i )
{
	m_userImage = i;
	zoomBy( 1 );
}

void PixmapViewer::wheelEvent ( QWheelEvent *e )
{
	if (m_bestFit)
		return;

	if (e->delta()>0)
		zoomBy( 1.1 );
	else
		zoomBy( 1/1.1 );
}

void PixmapViewer::paintEvent ( QPaintEvent *event )
{
	Q_UNUSED(event);
	if (m_userImage.isNull())
	{
		return;
	}

	QPainter p( this );
	int xoffset, yoffset;
	int sizeX = m_displayPixmap.width(); // of image...
	int sizeY = m_displayPixmap.height();
	
	if (width()  > sizeX)
		xoffset =  (width()  - sizeX) /2;
	else
		xoffset = 0;

	if (height() > sizeY)
		yoffset = (height() - sizeY)/2;
	else
		yoffset = 0;

	p.drawPixmap( xoffset, yoffset, m_displayPixmap );


	if (yoffset || xoffset != 0)
	{
		p.setPen( Qt::black );
		p.drawRect( xoffset-1, yoffset-1, sizeX, sizeY );
	}
}

void PixmapViewer::resizeEvent( QResizeEvent * event )
{
	Q_UNUSED(event);
	if ((!m_bestFit) || m_userImage.isNull())
		return;

	zoomBy( 1 );
}

void PixmapViewer::zoomBy( float f )
{
	if (m_userImage.isNull())
		return;

	// sanity checks
	if (m_zoomFactor*f>4.0)
		return;

	// 0 is treated as rest zoom
	if ((f!=0.0) && (m_zoomFactor*f<0.25))
		return;

	m_zoomFactor *= f;
	if (m_zoomFactor == 0)
		m_zoomFactor = 1;

	if (m_bestFit)
		// Qt is still too slow, when it will be faster, I will 
		// revert the zoom to smooth, meanwhile I will keep it at fast
		m_displayPixmap = QPixmap::fromImage( m_userImage.scaled( size(), Qt::KeepAspectRatio) );
	else
		m_displayPixmap = QPixmap::fromImage(
			m_userImage.scaled( m_userImage.size() * m_zoomFactor,
					Qt::KeepAspectRatioByExpanding,
					Qt::SmoothTransformation
			)
		);
	
	if (!m_bestFit)
	{
		setMinimumSize( m_displayPixmap.width(), m_displayPixmap.height() );
		QWidget *p = dynamic_cast<QWidget*>( parent() );
		if (p)
			resize( p->width(), p->height() );
	}
	
	repaint();
}

void PixmapViewer::setBestFit( bool a )
{
	m_bestFit = a;
	if (a)
	{
		QWidget *p = dynamic_cast<QWidget*>( parent() );
		setMinimumSize( 32, 32 );
		resize( p->size() );
// 		setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}
// 	else
// 		setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		//setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		//setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	zoomBy(1);
}
