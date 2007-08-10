#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QThread>

#include "hocr.h"
#include "pixmapviewer.h"
#include "ui_hocr_options.h"
#include "ui_qhocr_mainwin.h"

class QImage;
class QString;
class QDialog;
class QScrollArea;

class HOCRThread: public QThread
{
public:
	HOCRThread( hocr_pixbuf *p,  hocr_text_buffer *t  );
	virtual void run();
	
private:
	hocr_pixbuf *pix;
	hocr_text_buffer *text;
};

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
	PixmapViewer *imageLabel;
	QScrollArea  *scrollArea;

	QImage       savedImage;
	QImage       scannedImage;
	QString      imageFilename;
	QDialog      optionsDialog;
	Ui::OptionsDialog ui;
	
	// hocr options - hocr_ocr_type
	bool hocr_ocr_type_nikud;
	bool hocr_output_with_graphics;
	int  hocr_brightness;

	int hocr_timer;
	HOCRThread *hocr_thread;
	hocr_pixbuf *hocr_pix;
	hocr_text_buffer *hocr_text;
};

#endif //__MAIN_WINDOW_H__
