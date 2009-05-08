#include <QCoreApplication>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include "qhocrthread.h"

int main( int argc, char *argv[] )
{
	QCoreApplication app(argc,argv);
//	QHOCRThread hocr("/home/elcuco/src/qhocr/tests/bible-01-small_print.jpg");
	QHOCRThread hocr("/home/elcuco/src/qhocr/tests/test6.jpg");
	
	hocr.mHOCR_font_options.font_code = 1;
	
	hocr.doOCR(); // just calls QThread::start();
	const int MAX_TIME = 60*3; // in seconds
	QTime t;
	t.start();
	while (hocr.isRunning())
	{
		hocr.usleep( 250 );
		if ( (t.elapsed() % 1000) == 0)
			qDebug( "Process after %dsec: %d%% at stage %d", t.elapsed()/1000, hocr.getProcess(), hocr.getStage() );
		if (t.elapsed() > MAX_TIME * 1000){
			hocr.terminate();
			hocr.quit();
			qDebug("Watch dog working - the process took more then %d seconds", MAX_TIME );
			break;
		}
	}
	
	qDebug() << "Found string:" << hocr.getString();
	qDebug("End of application");
//	return app.exec();
}
