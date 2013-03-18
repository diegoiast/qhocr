#include <QApplication>
#include <QMainWindow>
#include <QString>
#include "mainwindow.h"

/**
 * \brief Entry point of the qhocr application
 * \arg argc Needed for compability
 * \arg argv Needed for compability
 *
 * This is the entry point of the application.
 * Constuct a QApplication, a MainWindow and execute the application.
 *
 */
int main(int argc, char *argv[])
{
	QApplication app(argc, argv);	
	app.setOrganizationName("Trolltech");
	app.setApplicationName("QHOCR");
	
	QString s = QCoreApplication::applicationDirPath() + "/plugins";
	app.addLibraryPath( s );
	
	MainWindow window;
	window.show();
	return app.exec();
}
