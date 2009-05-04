#ifndef QHOCRTHREAD_H
#define QHOCRTHREAD_H

#include <QThread>
#include <QImage>

class QHOCRThread : public QThread{
	Q_OBJECT
public:
	QHOCRThread( QString );
	
	void run();
private:
	QString mFileName;
	QImage mImage;
};


#endif // QHOCRTHREAD_H
