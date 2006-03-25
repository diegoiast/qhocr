#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QLabel>
#include <QScrollArea>
#include <QDockWidget>
#include <QStatusBar>
#include <QTextEdit>
#include <QPixmap>

#include "hocr.h"
#include "pixmapviewer.h"
#include "ui_hocr_options.h"

class MainWindow: public QMainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent=0 );
	void createActions();
	void createMenus();
	void createToolbars();

public slots:
	void on_aboutButton_clicked();
	void on_exitButton_clicked();
	void on_loadButton_clicked();
	void on_saveButton_clicked();
	void on_zoomInButton_clicked();
	void on_zoomOutButton_clicked();
	void on_zoomNormalButton_clicked();
	void on_changeFont_clicked();
	void on_bestFit_clicked();
	void on_options_clicked();
	void apply_hocr_settings();
	void saveStatus();
	void loadStatus();
	void closeEvent(QCloseEvent *event);
	
	void doOCR();
	void updateTimer();
	virtual void timerEvent();

private:
	void viewImage( QString fileName );
	bool saveHTML( QString fileName, QString text );
	bool saveText( QString fileName, QString text, bool unicode );

	QAction     *actionAbout;
	QAction     *actionAboutQt;
	QAction     *actionLoadImage;
	QAction     *actionSaveText;
	QAction     *actionZoomIn;
	QAction     *actionZoomOut;
	QAction     *actionZoomNormal;
	QAction     *actionChangeFont;
	QAction     *actionOptions;
	QAction     *actionExit;
	QAction     *actionBestFit;

	QToolBar    *tbFile;
	QToolBar    *tbView;
	QToolBar    *fileToolBar;
	QMenu       *menuFile;
	QMenu       *menuView;
	QMenu       *menuHelp;
	QDockWidget *dwTextArea;
	QTextEdit   *scannedText;	
	PixmapViewer *imageLabel;
	QScrollArea  *scrollArea;

	QImage       savedImage;
	QImage       scannedImage;
	QString      imageFilename;
	QDialog      optionsDialog;
	Ui::OptionsDialog ui;
	
	// hocr options - hocr_ocr_type
	bool hocr_ocr_type_regular;
	bool hocr_ocr_type_columns;
	bool hocr_ocr_type_nikud;
	bool hocr_ocr_type_table;
	bool hocr_ocr_type_no_font_recognition;

	// hocr_options - hocr_output;
	bool hocr_output_just_ocr;
	bool hocr_output_with_graphics;
	bool hocr_output_with_debug_text;

	int hocr_brightness;
	hocr_pixbuf *hocr_pix;
};

#endif //__MAIN_WINDOW_H__
