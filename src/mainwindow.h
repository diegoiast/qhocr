#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QThread>
#include <QHash>

#include "hocr.h"
#include "pixmapviewer.h"
#include "ui_hocr_options.h"
#include "ui_qhocr_mainwin.h"

class QImage;
class QString;
class QDialog;
class QScrollArea;

class MainWindow: public QMainWindow, public Ui::qhocrMainWindow 
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent=0 );

public slots:
	void on_actionAbout_triggered();
	void on_action_Quit_triggered();
	void on_action_Open_triggered();
	void on_action_Save_triggered();
	void on_actionZoomIn_triggered();
	void on_actionZoomOut_triggered();
	void on_actionZoomNormal_triggered();
	void _on_changeFont_clicked();
	void on_actionBestFit_triggered();
	void on_actionHOCR_Preferences_triggered();
	void apply_hocr_settings();
	void saveStatus();
	void loadStatus();
	void closeEvent(QCloseEvent *);
	
	void doOCR();
	virtual void timerEvent(QTimerEvent *);

private:
	void viewImage( QString fileName );
	bool saveHTML( QString fileName, QString text );
	bool saveText( QString fileName, QString text, bool unicode );
	PixmapViewer	*imageViewer;
	QScrollArea	*scrollArea;
	QString		imageFilename;
	QImage		imageToOCR;

	QDialog      optionsDialog;
	Ui::OptionsDialog ui;
	
	// this is *so* lame
	QString saveMessage;
	QHash<QString,QString> extByMessage;
	
	int hocr_timer;
};

#endif //__MAIN_WINDOW_H__
