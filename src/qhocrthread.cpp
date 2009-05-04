#include "qhocrthread.h"
#include "hocr.h"


QHOCRThread::QHOCRThread( QString fileName )
{
	mImage.load( fileName );
	mFileName    = fileName;
	
	// HOCR variables
	mHOCR_pixbuf = NULL;
	mHOCR_text   = NULL;
	mHOCR_progress = 0;
}

void QHOCRThread::run()
{
	if (mHOCR_pixbuf != NULL){
		// this means another thread is running....
		return;
	}
	
	const unsigned int channels  = mImage.depth() / 8;
	const unsigned int width     = mImage.width();
	const unsigned int height    = mImage.height();
	const int          rowstride = mImage.bytesPerLine();
	
	// note how ugly C is, as I could not forward declard ho_pixbuf and
	// had to use a typecast...
	mHOCR_pixbuf = (ho_pixbuf*) ho_pixbuf_new( channels, width, height, rowstride );
	mHOCR_text   = (ho_string*) ho_string_new ();
	// move the pixels from the Qt image to the HOCR pixbuf
	for( unsigned int x=0; x<width; x++ )
		for( unsigned int y=0; y<height; y++ )
			ho_pixbuf_set( (ho_pixbuf*) mHOCR_pixbuf, x, y, 0, 0 );
	
	hocr_do_ocr( (ho_pixbuf*) mHOCR_pixbuf, (ho_string*)mHOCR_text, 1, 0, 0, &mHOCR_progress );
	
	//exec();
}

void QHOCRThread::doOCR()
{
	this->start();
}

void QHOCRThread::usleep(unsigned int i)
{
	QThread::usleep(i);
}
