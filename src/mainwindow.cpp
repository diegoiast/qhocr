#include <QApplication>
#include <QMessageBox>
#include <QPixmap>
#include <QFileDialog>
#include <QTextStream>
#include <QTextCodec>
#include <QFontDialog>
#include <QSettings>
#include <QTime>
#include <QtDebug>
#include <QThread>
#include <QTimer>

#include "mainwindow.h"
#include "ui_hocr_options.h"

#define TITLE "QHOCR 0.7.0"

HOCRThread::HOCRThread( hocr_pixbuf *p,  hocr_text_buffer *t  )
{
	pix = p;
	text = t;
}

void HOCRThread::run()
{
	hocr_do_ocr (pix, text);
}


MainWindow::MainWindow( QWidget *parent ):QMainWindow( parent )
{
	createActions();
	createMenus();
	createToolbars();

	// main interface
	statusBar()->showMessage( tr("Welcome - load an image to start"), 5000 );
	statusBar()->setSizeGripEnabled( true );

	dwTextArea = new QDockWidget("Recognized text");
	dwTextArea->setObjectName("Recognized text");
	scannedText = new QTextEdit;
	dwTextArea->setWidget( scannedText );
	scannedText->setReadOnly( true );
	scannedText->setLayoutDirection( Qt::RightToLeft );
	addDockWidget( Qt::BottomDockWidgetArea, dwTextArea );

	imageLabel = new PixmapViewer;
	scrollArea = new QScrollArea;
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(imageLabel);
	scrollArea->setWidgetResizable( true );
//	scrollArea->setLayoutDirection( Qt::RightToLeft );
	setCentralWidget(scrollArea);

	// non-modal options dialog
	ui.setupUi(&optionsDialog);
 	connect(ui.applyButton, SIGNAL(clicked()), this, SLOT(apply_hocr_settings()));

	// set default options for viewing images
	hocr_pix = NULL;
	hocr_ocr_type_regular = true;
	hocr_ocr_type_columns = true;
	hocr_ocr_type_nikud = true;
	hocr_ocr_type_table = true;
	hocr_ocr_type_no_font_recognition = false;
	hocr_output_just_ocr = false;
	hocr_output_with_graphics = false;
	hocr_output_with_debug_text = false;
	hocr_brightness = 100;

	setWindowTitle( TITLE );
	hocr_timer = 0;
	hocr_thread = NULL;
	loadStatus();
	
//	viewImage( "tests/test3.jpg" );
}

void MainWindow::createActions()
{
	actionLoadImage  = new QAction( QIcon(":/src/images/open.png"), tr("Open..."), this );
	actionSaveText   = new QAction( QIcon(":/src/images/save.png"), tr("Save..."), this );
	actionZoomIn     = new QAction( QIcon(":/src/images/zoomin.png"), tr("Zoom in"), this );
	actionZoomOut    = new QAction( QIcon(":/src/images/zoomout.png"), tr("Zoom out"), this );
	actionChangeFont = new QAction( tr("Change font"), this );
	actionZoomNormal = new QAction( tr("1:1"), this );
	actionExit       = new QAction( tr("Quit"), this );
	actionAbout      = new QAction( tr("About..."), this );
	actionBestFit    = new QAction( tr("Best fit"), this );
	actionAboutQt    = new QAction(tr("About &Qt"), this);
	actionOptions    = new QAction(tr("HOCR &Preferences"), this);

	actionLoadImage ->setShortcut(tr("Ctrl+O"));
	actionSaveText  ->setShortcut(tr("Ctrl+S"));
	actionZoomIn    ->setShortcut(tr("Ctrl++"));
	actionZoomOut   ->setShortcut(tr("Ctrl+-"));
	actionZoomNormal->setShortcut(tr("Ctrl+="));
	actionExit      ->setShortcut(tr("Ctrl+Q"));
	actionOptions   ->setShortcut(tr("Ctrl+H"));
	actionBestFit   ->setShortcut(tr("Ctrl+A"));
	actionBestFit   ->setCheckable( true );
	actionBestFit   ->setChecked(false);

	actionAboutQt->setStatusTip(tr("Show the Qt library's About box"));
	
	connect( actionLoadImage , SIGNAL(triggered()), this, SLOT(on_loadButton_clicked()) );
	connect( actionSaveText  , SIGNAL(triggered()), this, SLOT(on_saveButton_clicked()) );
	connect( actionExit      , SIGNAL(triggered()), this, SLOT(on_exitButton_clicked()) );
	connect( actionZoomIn    , SIGNAL(triggered()), this, SLOT(on_zoomInButton_clicked()) );
	connect( actionZoomOut   , SIGNAL(triggered()), this, SLOT(on_zoomOutButton_clicked()) );
	connect( actionChangeFont, SIGNAL(triggered()), this, SLOT(on_changeFont_clicked()) );
	connect( actionZoomNormal, SIGNAL(triggered()), this, SLOT(on_zoomNormalButton_clicked()) );
	connect( actionAbout     , SIGNAL(triggered()), this, SLOT(on_aboutButton_clicked()) );
	connect( actionBestFit   , SIGNAL(triggered()), this, SLOT(on_bestFit_clicked()) );
	connect( actionOptions   , SIGNAL(triggered()), this, SLOT(on_options_clicked()) );

	connect( actionAboutQt   , SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	// pixmap viewer, is on max aspect mode by default
	actionZoomNormal->setEnabled( false );
	actionZoomIn->setEnabled( false );
	actionZoomOut->setEnabled( false );
	actionZoomNormal->setEnabled( false );
}

void MainWindow::createMenus()
{
	menuFile = menuBar()->addMenu(tr("&File"));
	menuFile->addAction( actionLoadImage );
	menuFile->addAction( actionSaveText );
	menuFile->addSeparator();
	menuFile->addAction( actionOptions );
	menuFile->addSeparator();
	menuFile->addAction( actionExit );

	menuView = menuBar()->addMenu(tr("&View"));
	menuView->addAction( actionZoomIn );
	menuView->addAction( actionZoomOut );
	menuView->addAction( actionZoomNormal );
	menuView->addSeparator();
	menuView->addAction( actionBestFit );
	menuView->addSeparator();
	menuView->addAction( actionChangeFont );

	menuHelp = menuBar()->addMenu(tr("&Help"));
	menuHelp->addAction( actionAbout );
	menuHelp->addAction( actionAboutQt );
}

void MainWindow::createToolbars()
{
	tbFile = addToolBar( tr("File operations") );
	tbFile->setObjectName( "File operations" );
	tbFile->addAction( actionLoadImage );
	tbFile->addAction( actionSaveText );

	tbView = addToolBar( tr("Image operations") );
	tbView->setObjectName( "Image operations" );
	tbView->addAction( actionZoomIn );
	tbView->addAction( actionZoomOut );
// 	tbView->addAction( actionZoomNormal );
}

void MainWindow::on_aboutButton_clicked()
{
	QMessageBox::information( 0,
"About QHOCR 0.7.0", "QHOCR - a Qt4 GUI front end to the HOCR library"
"<br>Diego Iastrubni &lt;elcuco@kde.org&gt; 2005"
"<br><br>This application is free software, released under the terms of GPL"
"read LICENSE.GPL for more intormation."
"<br>This application uses <b>libhocr 0.7</b> by Kobi Zamir &lt;kzamir@walla.co.il&gt; http://hocr.berlios.de"
);
}

void MainWindow::on_exitButton_clicked()
{
	QApplication::exit(0);
}

void MainWindow::on_loadButton_clicked()
{
	QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a scanned (300dpi) image",
                    "",
                    "Images (*.png *.jpg *.jpeg *.bmp *.gif *.pnm *.xpm)");
	
	if (s.isEmpty())
		return;
	
	viewImage( s );
}

void MainWindow::on_saveButton_clicked()
{
	QString s = QFileDialog::getSaveFileName(
                    this,
                    "Choose a file to save to",
                    "",
                    "Text files (*.txt *.utf8 *.html *.htm)");

	if (s.isEmpty())
		return;
	
	bool status;
	if (s.endsWith(".html") || s.endsWith(".htm"))
		status = saveHTML( s, scannedText->toPlainText() );
	else if (s.endsWith(".utf8"))
		status = saveText( s, scannedText->toPlainText(), true );
	else
		status = saveText( s, scannedText->toPlainText(), false );

	if (status)
		statusBar()->showMessage( tr("Text saved"), 5000 );
	else
		statusBar()->showMessage( tr("An error occured while saving the text"), 5000 );
}

void MainWindow::on_zoomInButton_clicked()
{
	imageLabel->zoomBy( 1.1 );
}

void MainWindow::on_zoomOutButton_clicked()
{
	imageLabel->zoomBy( 1 / 1.1 );
}

void MainWindow::on_zoomNormalButton_clicked()
{
	imageLabel->zoomBy( 0 );
}

void MainWindow::on_changeFont_clicked()
{
	bool ok;
	QFont font = QFontDialog::getFont( &ok, scannedText->font(), this );

	if (ok)
		scannedText->setFont( font );
}

void MainWindow::on_bestFit_clicked()
{
	bool b = imageLabel->getBestFit();
	imageLabel->setBestFit( !b );

	actionZoomNormal->setEnabled( b );
	actionZoomIn->setEnabled( b );
	actionZoomOut->setEnabled( b );
	actionZoomNormal->setEnabled( b );
}

void MainWindow::on_options_clicked()
{
	ui.cbRegular->setChecked(hocr_ocr_type_regular);
	ui.cbColumns->setChecked(hocr_ocr_type_columns);
	ui.cbNikud->setChecked(hocr_ocr_type_nikud);
	ui.cbTable->setChecked(hocr_ocr_type_table);
	ui.cbNoFont->setChecked(hocr_ocr_type_no_font_recognition);

	ui.cbJustOCR->setChecked(hocr_output_just_ocr);
	ui.cbGraphics->setChecked(hocr_output_with_graphics);
	ui.cbDebug->setChecked(hocr_output_with_debug_text);

	ui.sliderBrightness->setValue(hocr_brightness);

	if (optionsDialog.isHidden())
		optionsDialog.show();
	else
		optionsDialog.hide();
}

void MainWindow::apply_hocr_settings()
{
	hocr_ocr_type_regular       = ui.cbRegular->isChecked();
	hocr_ocr_type_columns       = ui.cbColumns->isChecked();
	hocr_ocr_type_nikud         = ui.cbNikud->isChecked();
	hocr_ocr_type_table         = ui.cbTable->isChecked();
	hocr_ocr_type_no_font_recognition = ui.cbNoFont->isChecked();

	hocr_output_just_ocr        = ui.cbJustOCR->isChecked();
	hocr_output_with_graphics   = ui.cbGraphics->isChecked();
	hocr_output_with_debug_text = ui.cbDebug->isChecked();

	hocr_brightness = ui.sliderBrightness->value();

	QTimer::singleShot( 0, this, SLOT(doOCR()));
}

void MainWindow::saveStatus()
{
	QSettings settings;
	settings.setValue( "main/image", imageFilename );
	settings.setValue( "main/size", size() );
	settings.setValue( "main/zoom", imageLabel->getZoomFactor() );
	settings.setValue( "main/bestfit", imageLabel->getBestFit() );
	settings.setValue( "main/position", pos() );
	settings.setValue( "main/state", saveState(0) );
	settings.setValue( "main/font", scannedText->font().toString() );
	settings.setValue( "hocr-type/regular", hocr_ocr_type_regular );
	settings.setValue( "hocr-type/columns", hocr_ocr_type_columns );
	settings.setValue( "hocr-type/nikud", hocr_ocr_type_nikud );
	settings.setValue( "hocr-type/table", hocr_ocr_type_table );
	settings.setValue( "hocr-type/font-recognition", hocr_ocr_type_no_font_recognition );
	settings.setValue( "hocr-output/just-ocr", hocr_output_just_ocr );
	settings.setValue( "hocr-output/graphics", hocr_output_with_graphics );
	settings.setValue( "hocr-output/debug", hocr_output_with_debug_text );
	settings.setValue( "hocr/brightness", hocr_brightness );

	settings.sync();
}

void MainWindow::loadStatus()
{
	QSettings settings;
	move( settings.value("main/position", QPoint(200, 200)).toPoint() );
	resize( settings.value("main/size", QSize(400, 400)).toSize() );
	restoreState( settings.value("main/state").toByteArray() );
	scannedText->setFont( QFont(settings.value("main/font").toString()) );

	hocr_ocr_type_regular       = settings.value( "hocr-type/regular", true ).toBool();
	hocr_ocr_type_columns       = settings.value( "hocr-type/columns", true ).toBool();
	hocr_ocr_type_nikud         = settings.value( "hocr-type/nikud", true  ).toBool();
	hocr_ocr_type_table         = settings.value( "hocr-type/table", true ).toBool();
	hocr_ocr_type_no_font_recognition  = settings.value( "hocr-type/font-recognition", false ).toBool();
	hocr_output_just_ocr        = settings.value( "hocr-output/just-ocr", false ).toBool();
	hocr_output_with_graphics   = settings.value( "hocr-output/graphics", false ).toBool();
	hocr_output_with_debug_text = settings.value( "hocr-output/debug", false ).toBool();
	hocr_brightness             = settings.value( "hocr/brightness", 100).toInt();

	bool bestFit = settings.value("main/bestfit", true).toBool();
	actionBestFit->setChecked( bestFit );
	actionZoomIn ->setEnabled( !bestFit );
	actionZoomOut->setEnabled( !bestFit );
	actionZoomNormal->setEnabled( !bestFit );
	imageLabel->setBestFit( bestFit );
	imageLabel->setZoomFactor( settings.value("main/zoom", "1").toDouble() );
		
	viewImage( settings.value( "main/image" ).toString() );
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	optionsDialog.hide();
	saveStatus();

	// ugly hack to shut up warnings...
	event = 0;
}

void MainWindow::viewImage( QString fileName )
{
	if (fileName.isEmpty())
		return;

	if (hocr_thread !=NULL)
	{
		hocr_thread->quit();
		while(hocr_thread->isRunning());
		delete hocr_thread;
		hocr_thread = NULL;
		
		scannedText->clear();
		hocr_pix->pixels = NULL;
		hocr_pixbuf_unref( hocr_pix );
		hocr_text_buffer_unref( hocr_text );
	}

	if (hocr_timer != 0)
	{
		killTimer(hocr_timer);
		hocr_timer = 0;
	}
	
	savedImage.load( fileName );
	QTimer::singleShot( 0, this, SLOT(doOCR()));
	
	setWindowTitle( QString(TITLE) + " - " + fileName );
	imageFilename = fileName;
}

bool MainWindow::saveHTML( QString fileName, QString text )
{
	QFile file( ":src/default.html" );
	file.open(QFile::ReadOnly | QFile::Text);
	
	file.setFileName( fileName );
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical( this, "Error", "Cound not save file" );
		return false;
	}

	QString html = file.readAll();
	html = QString(html).arg("File created by QHOCT - a Qt4 GUI for HOCR").arg( text );
	QTextStream out(&file);
	out.setCodec(QTextCodec::codecForName("UTF-8"));
	out << html;

	return true;
}

bool MainWindow::saveText( QString fileName, QString text, bool unicode )
{
	QFile file( fileName );
	
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical( this, "Error", "Cound not save file" );
		return false;
	}

	QTextStream out(&file);
	out.setCodec( QTextCodec::codecForName(unicode?"UTF-8":"Windows 1255") );
	out << text;
	return true;
}

void MainWindow::doOCR()
{
	hocr_pix = hocr_pixbuf_new();
	hocr_text = hocr_text_buffer_new();
	
	scannedImage = savedImage;
	imageLabel->setImage( scannedImage );
		
	hocr_pix->command = 0;
	hocr_pix->command |= HOCR_COMMAND_OCR;
	
// 	if (hocr_ocr_type_regular)
// 	if (hocr_ocr_type_columns)
// 	if (hocr_ocr_type_table)
// 	if (hocr_ocr_type_no_font_recognition)
// 	if (hocr_output_just_ocr)
// 	if (hocr_output_with_debug_text)
	
	hocr_pix->command |= HOCR_COMMAND_DICT;
// 	hocr_pix->command |= HOCR_COMMAND_USE_SPACE_FOR_TAB;
// 	hocr_pix->command |= HOCR_COMMAND_USE_INDENTATION;
	
	if (hocr_ocr_type_nikud)
		hocr_pix->command |= HOCR_COMMAND_NIKUD;
	
	if (hocr_output_with_graphics)
	{
		hocr_pix->command |= HOCR_COMMAND_COLOR_BOXES;
		hocr_pix->command |= HOCR_COMMAND_COLOR_MISREAD;
	}

	hocr_pix->n_channels	= scannedImage.depth() / 8;
	hocr_pix->height	= scannedImage.size().height();
	hocr_pix->width		= scannedImage.size().width();
	hocr_pix->rowstride	= scannedImage.bytesPerLine();
	hocr_pix->pixels	= scannedImage.bits();
	hocr_pix->brightness    = hocr_brightness;

#if 1
	scannedText->clear();
	hocr_thread = new HOCRThread( hocr_pix, hocr_text );
	hocr_thread->start();
	QTimer::singleShot( 0, this, SLOT(doOCR_async()));
	hocr_timer = startTimer( 50 );
#else
	statusBar()->showMessage( "Processing image started..." );
	hocr_do_ocr (hocr_pix, hocr_text);
	imageLabel->setImage( scannedImage );
	scannedText->setPlainText( QString::fromUtf8(hocr_text->text) );
	hocr_pix->pixels = NULL;
	hocr_pixbuf_unref( hocr_pix );
	hocr_text_buffer_unref( hocr_text );
	hocr_pix = NULL;
	statusBar()->showMessage( "Processing image done", 5000 );
#endif
}

void MainWindow::timerEvent(QTimerEvent *event)
{
	if (hocr_pix == NULL)
		return;

	int percentage = (int)(100*(hocr_pix->progress+2)/256.);
	statusBar()->showMessage( QString("Processing image: %1%").arg(percentage), 3000 );
	scannedText->setPlainText( QString::fromUtf8(hocr_text->text) );

	if (hocr_thread->isFinished())
	{
		imageLabel->setImage( scannedImage );

		scannedText->setPlainText( QString::fromUtf8(hocr_text->text) );
		hocr_pix->pixels = NULL;
		hocr_pixbuf_unref( hocr_pix );
		hocr_text_buffer_unref( hocr_text );
		hocr_pix = NULL;
		statusBar()->showMessage( "Processing image done", 5000 );

		killTimer( hocr_timer );
		hocr_timer = 0;
		delete hocr_thread;
		hocr_thread = NULL;
	}
	
	// ugly hack to shut up warnings
	event = 0;
}
