#ifndef QHOCRTHREAD_H
#define QHOCRTHREAD_H

#include <QThread>
#include <QImage>

#include "hocr.h"

typedef struct HOCR_FONT_OPTIONS {
	int font_code;
	unsigned char html;
	unsigned char nikud;
	unsigned char do_linguistics;
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
	QHOCRThread();
	QHOCRThread( QImage );
	QHOCRThread( QString );

	void setDefaultSettings();
	void setImage( QImage  );
	void loadFile( QString );
	void doOCR();

// this will do the OCR in a separate thread
	virtual void run();

// inline functions, used to retrive internal structrure
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

// set as public - read only when you call doOCR()
	HEBOCR_IMAGE_OPTIONS mHOCR_image_options;
	HEBOCR_LAYOUT_OPTIONS  mHOCR_layout_options;
	HOCR_FONT_OPTIONS    mHOCR_font_options;

	void* getPixbufFromQImage( QImage * );
signals:
	void stageChanged(HOCR_STAGES::stageNames newStage);
	
private:
	
	QString mParsedText;
	QString mFileName;
	QImage  mImage;
	
	HOCR_STAGES::stageNames mStage;
	int mHOCR_progress; // process is inside each stage
	
};


  int getMemSize();

#endif // QHOCRTHREAD_H
