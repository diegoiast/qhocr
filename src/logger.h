#ifndef __LOGGER__H__
#define __LOGGER__H__

class QString;
class QIODevice;

class Logger
{
public:	
	int debug( QString s, QString fileName, int line=-1, int row=-1, int level=0 );
	int info( QString s, QString fileName, int line=-1, int row=-1, int level=0 );
	
	Logger*		getInstance();
	void		setLogDevice( QString fileName );
	QIODevice*	getLogDevice();
	void		setLoggingLevel( int i );
	int		getLoggingLevel();
	void		setDebugLevel( int i );
	int		getDebugLevel();
	
private:
	Logger();
	~Logger();
	
	int		output( QString prefix, QString s, QString fileName, int line, int row );
		
	int 		debugLevel;
	int 		loggingLevel;
	static Logger		*instance;
	QIODevice	*logDevice;
};

#endif //__LOGGER__H__
