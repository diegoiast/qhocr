#include <QPainter>
#include <QWheelEvent>
#include <QDebug>
#include "pixmapviewer.h"

PixmapViewer::PixmapViewer():QWidget()
{
	setBackgroundRole(QPalette::Dark);
//	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
// 	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	zoomFactor = 1;
	bestFit = true;
	userImage = QImage();
}

PixmapViewer::PixmapViewer( QWidget *parent ):QWidget(parent)
{
	setBackgroundRole(QPalette::Base);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
 	//setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	bestFit = true;
	zoomFactor = 1;
}

PixmapViewer::~PixmapViewer()
{
}

void PixmapViewer::loadImage( QString f )
{
	userImage = QImage( f );
	zoomBy( 1 );
}

void PixmapViewer::setPixmap( QPixmap p )
{
	userImage = p.toImage();
	zoomBy( 1 );
}

void PixmapViewer::setImage( QImage i )
{
	userImage = i;
	zoomBy( 1 );
}

void PixmapViewer::wheelEvent ( QWheelEvent *e )
{
	if (bestFit)
		return;

	if (e->delta()>0)
		zoomBy( 1.1 );
	else
		zoomBy( 1/1.1 );
}

void PixmapViewer::paintEvent ( QPaintEvent *event )
{
	if (userImage.isNull())
	{
		return;
	}

	QPainter p( this );
	int xoffset, yoffset;
	int sizeX = displayPixmap.width(); // of image...
	int sizeY = displayPixmap.height();
	
	if (width()  > sizeX)
		xoffset =  (width()  - sizeX) /2;
	else
		xoffset = 0;

	if (height() > sizeY)
		yoffset = (height() - sizeY)/2;
	else
		yoffset = 0;

	p.drawPixmap( xoffset, yoffset, displayPixmap );


	if (yoffset || xoffset != 0)
	{
		p.setPen( Qt::black );
		p.drawRect( xoffset-1, yoffset-1, sizeX, sizeY );
	}
}

void PixmapViewer::resizeEvent( QResizeEvent * event )
{
	if ((!bestFit) || userImage.isNull())
		return;

	zoomBy( 1 );
}

void PixmapViewer::zoomBy( float f )
{
	if (userImage.isNull())
		return;

	// sanity checks
	if (zoomFactor*f>4.0)
		return;

	// 0 is treated as rest zoom
	if ((f!=0.0) && (zoomFactor*f<0.25))
		return;

	zoomFactor *= f;
	if (zoomFactor == 0)
		zoomFactor = 1;

	if (bestFit)
		// Qt is still too slow, when it will be faster, I will 
		// revert the zoom to smooth, meanwhile I will keep it at fast
		displayPixmap = QPixmap::fromImage( userImage.scaled( size(), Qt::KeepAspectRatio) );
	else
		displayPixmap = QPixmap::fromImage(
			userImage.scaled( userImage.size() * zoomFactor,
					Qt::KeepAspectRatioByExpanding,
					Qt::SmoothTransformation
			)
		);
	
	if (!bestFit)
	{
		setMinimumSize( displayPixmap.width(), displayPixmap.height() );
		QWidget *pp = dynamic_cast<QWidget*>( parent() );
		if (pp)
			resize( pp->width(), pp->height() );
	}
	
	repaint();
}

void PixmapViewer::setBestFit( bool a )
{
	bestFit = a;
	if (a)
	{
		QWidget *pp = dynamic_cast<QWidget*>( parent() );
		setMinimumSize( 32, 32 );
		resize( pp->size() );
// 		setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	}
// 	else
// 		setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		//setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		//setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	zoomBy(1);
};
