#include <QCoreApplication>
#include <QTimer>
#include <QTime>
#include <QDebug>

#include "qhocrthread.h"


int main( int argc, char *argv[] )
{
	QCoreApplication app(argc,argv);
	
	QHOCRThread hocr("/home/elcuco/src/qhocr/tests/bible-01-small_print.jpg");
//	hocr.run();
//	hocr.start();
	hocr.doOCR();
	
	QTime t;
	t.start();
	
	while (hocr.isRunning())
	{
		hocr.usleep( 50 );
		
		if ( (t.elapsed() % 1000) == 0)
			qDebug() << t.elapsed() << hocr.getProcess();
		
		
		// idle loop;
		if (t.elapsed() > 15000){
			hocr.exit();
			qDebug("Watch dog working - the process took more then 5 seconds");
			break;
		}
	}
	
	
	qDebug("Hello world");
	return app.exec();
}

