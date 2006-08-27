#include <QString>
#include <QIODevice>
#include <QFile>

#include "logger.h"


Logger *Logger::instance = 0;

int Logger::debug( QString s, QString fileName, int line, int row, int level )
{
	if (level< debugLevel)
		return 0;
		
	return output( "DEBUG", s, fileName, line, row );
}

int Logger::info( QString s, QString fileName, int line, int row, int level )
{
	if (level< loggingLevel)
		return 0;
	
	return output( "INFO", s, fileName, line, row );
}

void Logger::setLogDevice( QString fileName )
{
	if (logDevice)
		delete logDevice;
	logDevice = NULL;
	
	logDevice = new QFile( fileName );
}

QIODevice* Logger::getLogDevice()
{
	return logDevice;
}
	
Logger *Logger::getInstance()
{
	if (!instance)
		instance = new Logger;
		
	return instance;
}

Logger::Logger()
{
	// output... welcome...
}

Logger::~Logger()
{
	delete logDevice;
}

int Logger::output( QString prefix, QString s, QString fileName, int line, int row )
{
	if (!logDevice)
	{
		qDebug( "Logger::output() error: no log device set!" );
		return -1;
	}
	
	if (!logDevice->isTextModeEnabled () )
	{
		qDebug( "Logger::output() error: log device does not support text!" );
		return -1;		
	}
	
	QString l;
	
	// do we have a file...?
	if (!fileName.isEmpty())
		l = QString(" %1").arg(s);
	else
		if (line<0)
			// no line number passed on
			l = QString(" %1 : %2").arg(fileName).arg(s);
		else
			// we have line, do we have row...?
			if (row<0)
				l = QString(" %1 [%2]: %3").arg(fileName).arg(s).arg(line);
			else
				l = QString(" %1 [%2:%3]: %4").arg(fileName).arg(s).arg(line).arg(row);	
			
				
	l = prefix + l;
	qint64 r = 0;
	r += logDevice->write( l.toUtf8() );
	r += logDevice->write( "\n\r", 3 );
	return r;
}
