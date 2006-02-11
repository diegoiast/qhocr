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
	void wheelEvent( QWheelEvent *e );
	void paintEvent( QPaintEvent *event );
	void resizeEvent( QResizeEvent * event );
	void zoomBy( float f );

	bool  getBestFit() { return bestFit; };
	void  setBestFit( bool a );
	float getZoomFactor() { return zoomFactor; };
	void  setZoomFactor( float f ) { zoomFactor=f; zoomBy(1); };
private:

	bool bestFit;
	QPixmap displayPixmap;
	QImage userImage;
	float zoomFactor;
};

#endif // __PIXMAP_VIEWER_H__

