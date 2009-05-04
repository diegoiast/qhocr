#include <QCoreApplication>
#include "qhocrthread.h"


int main( int argc, char *argv[] )
{
	QCoreApplication app(argc,argv);
	
	QHOCRThread hocr("/home/elcuco/src/qhocr/tests/bible-01-small_print.jpg");
	
	qDebug("Hello world");
	return app.exec();
}

