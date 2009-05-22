#ifndef QHOCRTHREAD_H
#define QHOCRTHREAD_H

#include <QThread>
#include <QImage>

typedef struct HOCR_IMAGE_OPTIONS{
	unsigned char scale;
	unsigned char no_auto_scale;
	double rotate;
	unsigned char no_auto_rotate;
	unsigned char adaptive;
	unsigned char threshold;
	unsigned char a_threshold;
} HOCR_IMAGE_OPTIONS;

typedef struct HOCR_LAYOUT_OPTIONS {
	int font_spacing_code;
	int paragraph_setup;
	int slicing_threshold;
	int slicing_width;
	unsigned char dir_ltr;
	unsigned char html;
} HOCR_OPTIONS;

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
	HOCR_IMAGE_OPTIONS  mHOCR_image_options;
	HOCR_LAYOUT_OPTIONS mHOCR_layout_options;
	HOCR_FONT_OPTIONS   mHOCR_font_options;

signals:
	void stageChanged(HOCR_STAGES::stageNames newStage);
	
private:
	void* getPixbufFromQImage( QImage * );
	
	QString mParsedText;
	QString mFileName;
	QImage  mImage;
	
	HOCR_STAGES::stageNames mStage;
	int mHOCR_progress; // process is inside each stage
	
};


  int getMemSize();

#endif // QHOCRTHREAD_H
