#ifndef __PIXMAP_VIEWER_H__
#define __PIXMAP_VIEWER_H__

#include <QWidget>
#include <QPixmap>
#include <QImage>
#include <QString>

class PixmapViewer: public QWidget
{
public:
	PixmapViewer();
	PixmapViewer( QWidget *parent );
	~PixmapViewer();
	void loadImage( QString f );
	void setPixmap( QPixmap p );
	void setImage( QImage i );
	QImage getImage(){ return m_userImage; }
	void wheelEvent( QWheelEvent *e );
	void paintEvent( QPaintEvent *event );
	void resizeEvent( QResizeEvent * event );
	void zoomBy( float f );

	bool  getBestFit() { return m_bestFit; }
	void  setBestFit( bool a );
	float getZoomFactor() { return m_zoomFactor; }
	void  setZoomFactor( float f ) { m_zoomFactor=f; zoomBy(1); }
private:

	bool	m_bestFit;
	QPixmap	m_displayPixmap;	// this is drawed on screen
	QImage	m_userImage;		// this is the original image
	float	m_zoomFactor;
};

#endif // __PIXMAP_VIEWER_H__

