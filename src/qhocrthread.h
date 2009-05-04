#ifndef QHOCRTHREAD_H
#define QHOCRTHREAD_H

#include <QThread>
#include <QImage>

class QHOCRThread : public QThread{
	Q_OBJECT
public:
	QHOCRThread( QString );
	virtual void run();
	
	void doOCR();
	void usleep(unsigned int);
	
	int getProcess(){ return mHOCR_progress; };
	
private:
	QString mFileName;
	QImage  mImage;
	
	void*   mHOCR_pixbuf;
	void*   mHOCR_text;
	int     mHOCR_progress;
};


#endif // QHOCRTHREAD_H
