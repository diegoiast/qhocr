#include "qhocrthread.h"


QHOCRThread::QHOCRThread( QString fileName )
{
	mImage.load( fileName );
	mFileName = fileName;
}

void QHOCRThread::run()
{
}

