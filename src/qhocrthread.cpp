#include <QRgb>
#include <QColor>

#include <cstdio>
#include "qhocrthread.h"
#include "hocr.h"

QHOCRThread::QHOCRThread()
{
	setDefaultSettings();
}

QHOCRThread::QHOCRThread( QImage i )
{
	setDefaultSettings();
	setImage( i );
}

QHOCRThread::QHOCRThread( QString f )
{
	setDefaultSettings();
	loadFile( f );
}

void QHOCRThread::setDefaultSettings()
{
	mFileName.clear();
	mImage		= QImage();
	mHOCR_progress	= 0;
	mStage		= HOCR_STAGES::idle;
	
	mHOCR_image_options.scale		= 0;
	mHOCR_image_options.auto_scale		= 1;
	mHOCR_image_options.rotation_angle	= 0;
	mHOCR_image_options.auto_rotate		= 1;
	mHOCR_image_options.adaptive		= 0;
	mHOCR_image_options.threshold		= 0;
	mHOCR_image_options.a_threshold		= 0;
	
	mHOCR_layout_options.font_spacing_code	= 0;
	mHOCR_layout_options.paragraph_setup	= 0;
	mHOCR_layout_options.slicing_threshold	= 0;
	mHOCR_layout_options.slicing_width	= 0;
	mHOCR_layout_options.line_leeway        = 0;
	mHOCR_layout_options.dir_ltr		= 0;
	mHOCR_layout_options.html		= 1;

	mHOCR_font_options.font_code		= 0;
	mHOCR_font_options.nikud		= 1;
	mHOCR_font_options.do_linguistics	= 0;
}

void QHOCRThread::setImage( QImage i )
{
	mImage = i;
	mFileName.clear();
}

void QHOCRThread::loadFile( QString f )
{
	mImage.load(f);
	mFileName = f;
}

void QHOCRThread::doOCR() 
{
	if (mStage != HOCR_STAGES::idle){
		// this means another thread is running....
		return;
	}
	
	mHOCR_progress = 0;
	mParsedText.clear();
	
	// Since we run in a thread, there is an option that the user
	// will modify the configuration, we still need to use the same
	// configuration. We copy the member's configuration locally
	// on this thread.
	HEBOCR_IMAGE_OPTIONS  image_options   = mHOCR_image_options;
	HEBOCR_LAYOUT_OPTIONS layout_options  = mHOCR_layout_options;
	HEBOCR_FONT_OPTIONS   font_options    = mHOCR_font_options;
	
	// phase 1 - image pre-processing
	mStage	= HOCR_STAGES::imagePreProces;
	emit stageChanged(mStage);
	ho_pixbuf *pixbuf  = (ho_pixbuf*)getPixbufFromQImage( &mImage );
	ho_bitmap *bitmap = hocr_image_processing( pixbuf,
		&image_options,
		&mHOCR_progress
	);
	if (!bitmap){
		// could not process image
		ho_pixbuf_free(pixbuf);
		return;
	}
	
	// phase 2 - layout analysis
	mStage	= HOCR_STAGES::layoutAnalysis;
	emit stageChanged(mStage);
	ho_layout *page = hocr_layout_analysis ( bitmap, &layout_options, &mHOCR_progress );
	if (!page){
		// could not do layout anlysis
		ho_pixbuf_free(pixbuf);
		ho_bitmap_free(bitmap);
		return;
	}
	
	// phase 3 - font recognition
	mStage	= HOCR_STAGES::fontRecognition;
	emit stageChanged(mStage);
	ho_string *text = ho_string_new();
	int return_val = hocr_font_recognition( page, text, &font_options, 1, &mHOCR_progress );
	
	mParsedText  = QString::fromUtf8( 
		((ho_string*) (text))->string, 
		((ho_string*) (text))->size
	);
	mStage	= HOCR_STAGES::idle;

	ho_layout_free(page);
	ho_pixbuf_free(pixbuf);
	ho_bitmap_free(bitmap);
	ho_string_free(text);
	exit(return_val);
}

void QHOCRThread::run()
{
	doOCR();
}

void* QHOCRThread::getPixbufFromQImage( QImage * img )
{
	if (!img)
		return NULL;

	ho_pixbuf* pix = ho_pixbuf_new(
		3,			// img->depth()/8, 
		img->width(), 
		img->height(), 
		0			// img->bytesPerLine()
	);
	for (int x=0; x < pix->width; x++)
		for(int y=0; y < pix->height; y++)
		{
			QColor color = img->pixel( x, y );
			ho_pixbuf_set( pix, x, y, 0, color.red() );
			ho_pixbuf_set( pix, x, y, 1, color.green() );
			ho_pixbuf_set( pix, x, y, 2, color.blue() );
		}
	return pix;
}

// http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
int parseLine(char* line){
	int i = strlen(line);
	while (*line != 9) line++;
	line[i-3] = '\0';
	i = atoi(line);
	return i;
}

int getMemSize(){ //Note: this value is in KB!
	FILE* file = fopen("/proc/self/status", "r");
	int result = -1;
	char line[128];
	
	while (fgets(line, 128, file) != NULL){
		if (strncmp(line, "VmRSS:", 6) == 0) {
			result = parseLine(line);
			break;
		}
	}
	fclose(file);
	return result;
}
