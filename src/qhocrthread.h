#ifndef QHOCRTHREAD_H
#define QHOCRTHREAD_H

#include <QThread>
#include <QImage>

typedef struct HOCR_IMAGE_OPTIONS{
	const unsigned char scale;
	const unsigned char no_auto_scale;
	double rotate;
	const unsigned char no_auto_rotate;
	const unsigned char adaptive;
	const unsigned char threshold;
	const unsigned char a_threshold;
} HOCR_IMAGE_OPTIONS;

typedef struct HOCR_LAYOUT_OPTIONS {
	const int font_spacing_code;
	const int paragraph_setup;
	const int slicing_threshold;
	const int slicing_width;
	const unsigned char dir_ltr;
	const unsigned char html;
} HOCR_OPTIONS;

typedef struct HOCR_FONT_OPTIONS {
	int font_code;
	const unsigned char html;
	const unsigned char nikud;
	const unsigned char do_linguistics;
} HOCR_FONT_OPTIONS;


// just because it's cool. Any better way of doing this?
namespace HOCR_STAGES {
	enum stageNames { 
		idle,		// phase 0 - doing nothing
		imagePreProces,	// phase 1 - image pre-processing
		layoutAnalysis,	// phase 2 - layout analysis
		fontRecognition	// phase 3 - font recognition
	};
}

class QHOCRThread : public QThread{
	Q_OBJECT
public:

	QHOCRThread( QString );
	virtual void run();
	
	void doOCR();
	void* getPixbufFromQImage( QImage * );
	
	int getProcess(){ 
		return mHOCR_progress; 
	}
	
	HOCR_STAGES::stageNames getStage(){ 
		return mStage; 
	}
	
	QString getString(){
		return mParsedText;
	}
	
	void usleep(unsigned int i){
		QThread::usleep(i);
	}

	HOCR_IMAGE_OPTIONS  mHOCR_image_options;
	HOCR_LAYOUT_OPTIONS mHOCR_layout_options;
	HOCR_FONT_OPTIONS   mHOCR_font_options;
	
private:
	QString mParsedText;
	QString mFileName;
	QImage mImage;
	
	HOCR_STAGES::stageNames mStage;
	int mHOCR_progress; // process is inside each stage
	
};


  int getMemSize();

#endif // QHOCRTHREAD_H
