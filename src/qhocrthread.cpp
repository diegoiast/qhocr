#include <QRgb>
#include <QColor>

#include "qhocrthread.h"
#include "hocr.h"

// memcopy
#include <stdio.h>


QHOCRThread::QHOCRThread( QString fileName )
{
	mImage.load( fileName );
	mFileName    = fileName;
	
	mHOCR_progress = 0;
	mStage         = HOCR_STAGES::idle;
	
	mHOCR_image_options.scale		= 0;
	mHOCR_image_options.no_auto_scale	= 0;
	mHOCR_image_options.rotate		= 0x00;
	mHOCR_image_options.no_auto_rotate	= 0;
	mHOCR_image_options.adaptive		= 0;
	mHOCR_image_options.threshold		= 0;
	mHOCR_image_options.a_threshold		= 0;
	
	mHOCR_layout_options.font_spacing_code	= 0;
	mHOCR_layout_options.paragraph_setup	= 0;
	mHOCR_layout_options.slicing_threshold	= 0;
	mHOCR_layout_options.slicing_width	= 0;
	mHOCR_layout_options.dir_ltr		= 0;
	mHOCR_layout_options.html		= 1;

	mHOCR_font_options.font_code		= 0;
	mHOCR_font_options.html			= 1;
	mHOCR_font_options.nikud		= 1;
	mHOCR_font_options.do_linguistics	= 0;
}

// CHANGE THIS TO 0 to use the other version
#if 1
// not working, the returned text is not recognized, I see only "*"
void QHOCRThread::run() 
{
	if (mStage != HOCR_STAGES::idle){
		// this means another thread is running....
		return;
	}
	
	//mHOCR_progress = 0;
	mParsedText.clear();
	
	// phase 1 - image pre-processing
	mStage	= HOCR_STAGES::imagePreProces;
	ho_pixbuf *pixbuf  = (ho_pixbuf*)getPixbufFromQImage( &mImage );
	ho_bitmap *bitmap = hocr_image_processing( pixbuf,
		mHOCR_image_options.scale,
		mHOCR_image_options.no_auto_scale,
		mHOCR_image_options.rotate,
		mHOCR_image_options.no_auto_rotate,
		mHOCR_image_options.adaptive,
		mHOCR_image_options.threshold,
		mHOCR_image_options.a_threshold,
		&mHOCR_progress
	);
	if (!bitmap){
		// could not process image
		ho_pixbuf_free(pixbuf);
		return;
	}
	
	// phase 2 - layout analysis
	mStage	= HOCR_STAGES::layoutAnalysis;
	ho_layout *page = hocr_layout_analysis ( bitmap,
		mHOCR_layout_options.font_spacing_code,
		mHOCR_layout_options.paragraph_setup,
		mHOCR_layout_options.slicing_threshold,
		mHOCR_layout_options.slicing_width,
		mHOCR_layout_options.dir_ltr,
		&mHOCR_progress
	);
	if (!page){
		// could not do layout anlysis
		ho_pixbuf_free(pixbuf);
		ho_bitmap_free(bitmap);
		return;
	}
	
	// phase 3 - font recognition
	mStage	= HOCR_STAGES::fontRecognition;
	ho_string *text = ho_string_new();
	int return_val = hocr_font_recognition( page, 
		text,
		mHOCR_font_options.html, 
		mHOCR_font_options.font_code, 
		mHOCR_font_options.nikud, 
		mHOCR_font_options.do_linguistics, 
		&mHOCR_progress
	);
	
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
#else
// working, but no control
void QHOCRThread::run()
{
	mStage	= HOCR_STAGES::fontRecognition;
	
	// note how ugly C is, as I could not forward declard ho_pixbuf and
	// had to use a typecast...
	ho_string *mHOCR_text    = (ho_string *)ho_string_new();
	ho_pixbuf *mHOCR_pixbuf  = (ho_pixbuf*) getPixbufFromQImage( &mImage );
	
	// this will take a long time...
	hocr_do_ocr( (ho_pixbuf*) mHOCR_pixbuf,		// pix_in
		     (ho_string*) mHOCR_text,		// s_text_out
		     1,					// html
		     0,					// font_code
		     0,					// do_linguistics
		     &mHOCR_progress 
	);

	qDebug("ocr ended at [%d]%%", mHOCR_progress);
	
	mParsedText  = QString::fromUtf8( 
		((ho_string*) (mHOCR_text))->string, 
		((ho_string*) (mHOCR_text))->size
	);
	free(mHOCR_text);
	free(mHOCR_pixbuf);
	mHOCR_text   = NULL;
	mHOCR_pixbuf = NULL;
	mStage	= HOCR_STAGES::idle;
	
	quit();
}
#endif

void QHOCRThread::doOCR()
{
	this->start();
}

void * QHOCRThread::getPixbufFromQImage( QImage * img )
{
	if (!img)
		return NULL;

	ho_pixbuf * pix = ho_pixbuf_new( img->depth()/8, 
		img->width(), 
		img->height(), 
		img->bytesPerLine()
	);
	for (int x=0; x<pix->width; x++)
		for(int y=0;y<pix->height; y++)
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
