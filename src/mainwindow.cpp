#include <QApplication>
#include <QScrollArea>
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
#include <QDir>
//#include <QFileInfo >

#include "mainwindow.h"
#include "ui_hocr_options.h"

#define TITLE "QHOCR 0.8.2-svn"

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
	setupUi( this );
	connect( actionAboutQt   , SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	imageLabel = new PixmapViewer;
	scrollArea = new QScrollArea;
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(imageLabel);
	scrollArea->setWidgetResizable( true );
//	scrollArea->setLayoutDirection( Qt::RightToLeft );
	setCentralWidget(scrollArea);

	statusBar()->showMessage( tr("Welcome - load an image to start"), 5000 );

	// non-modal options dialog
	ui.setupUi(&optionsDialog);
	connect(ui.btnTextFont, SIGNAL(clicked()), this, SLOT(_on_changeFont_clicked()) );
	connect(ui.applyButton, SIGNAL(clicked()), this, SLOT(apply_hocr_settings()));

	// set default options for viewing images
	hocr_pix = NULL;
	hocr_ocr_type_nikud = true;
	hocr_output_with_graphics = false;
	hocr_brightness = 100;

	setWindowTitle( TITLE );
	hocr_timer = 0;
	hocr_thread = NULL;
	loadStatus();
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::information( 0,
		"About QHOCR 0.8.2", "QHOCR - a Qt4 GUI front end to the HOCR library"
		"<br>Diego Iastrubni &lt;<a href='elcuco@kde.org'>elcuco@kde.org</a>&gt; 2005,2006"
		"<br><br>This application is free software, released under the terms of GPL"
		"read LICENSE.GPL for more intormation. Newer versions can be found at"
		" <a href='http://code.google.com/p/qhocr/'>http://code.google.com/p/qhocr/</a> <br>"
		"<br>This application uses <b>libhocr 0.8.2</b> by "
		"Kobi Zamir &lt;<a href='kzamir@walla.co.il'>kzamir@walla.co.il</a>&gt;, "
		"which can be found at <a href='http://hocr.berlios.de'>http://hocr.berlios.de</a>"
	);
}

void MainWindow::on_action_Quit_triggered()
{
	QApplication::exit(0);
}

void MainWindow::on_action_Open_triggered()
{
	statusBar()->showMessage("Loading file");
	QFileInfo info(imageFilename);
	QString lastDir = info.absolutePath();
	
	if (lastDir.isEmpty())
		lastDir = QDir::home().path();

	QString s = QFileDialog::getOpenFileName(
		this,
		tr("Choose a scanned (300dpi) image"),
		lastDir, 
		tr("Images (*.png *.jpg *.jpeg *.bmp *.gif *.pnm *.xpm)")
	);
	
	if (s.isNull())
	{
		statusBar()->showMessage("openning aborted", 3000 );
		return;
	}
	
	viewImage( s );
}

void MainWindow::on_action_Save_triggered()
{
	static QString sf;
	static QString lastSave;
	QFileInfo info(lastSave);
	QString lastDir = info.absolutePath();

	if (lastDir.isEmpty())
		lastDir = QDir::home().path();
	
	QString s = QFileDialog::getSaveFileName(
		this,
		"Choose a file to save to",
		lastDir,
		"Text files (*.txt *.utf8);; HTML Files (*.html *.htm)",
		&sf
	);

	if (s.isEmpty())
		return;
	
	bool status;
	QString s_lower = s.toLower();
	
	// what if the users did not specify a suffix...?
	QFileInfo f( s );
	if (f.suffix().isEmpty())
	{
		// waiting for ideas from:
		// http://www.qtcentre.org/forum/f-qt-programming-2/t-qfiledialoggetsavefilename-default-extension-8503.html
		qDebug("no suffix, adding %s", qPrintable( sf ) );
	}
	
	if (s_lower.endsWith(".html") || s_lower.endsWith(".htm"))
		status = saveHTML( s, scannedText->toPlainText() );
	else if (s_lower.endsWith(".utf8"))
		status = saveText( s, scannedText->toPlainText(), true );
	else
		status = saveText( s, scannedText->toPlainText(), false );

	if (status)
	{
		statusBar()->showMessage( tr("Text saved"), 5000 );
		lastSave = s;
	}
	else
		statusBar()->showMessage( tr("An error occured while saving the text"), 5000 );
}

void MainWindow::on_actionZoomIn_triggered()
{
	imageLabel->zoomBy( 1.1 );
}

void MainWindow::on_actionZoomOut_triggered()
{
	imageLabel->zoomBy( 1 / 1.1 );
}

void MainWindow::on_actionZoomNormal_triggered()
{
	imageLabel->zoomBy( 0 );
}

void MainWindow::_on_changeFont_clicked()
{
	optionsDialog.hide();
	bool ok;
	QFont font = QFontDialog::getFont( &ok, scannedText->font(), this );

	if (ok)
	{
		ui.textFontPreview->setFont( font );
		ui.textFontPreview->setText( font.family() );
	}
	
	optionsDialog.show();
}

void MainWindow::on_actionBestFit_triggered()
{
	bool b = imageLabel->getBestFit();
	imageLabel->setBestFit( !b );

	actionZoomNormal->setEnabled( b );
	actionZoomIn->setEnabled( b );
	actionZoomOut->setEnabled( b );
	actionZoomNormal->setEnabled( b );
}

void MainWindow::on_actionHOCR_Preferences_triggered()
{
	ui.cbNikud->setChecked(hocr_ocr_type_nikud);
	ui.cbGraphics->setChecked(hocr_output_with_graphics);

	ui.sliderBrightness->setValue(hocr_brightness);

	if (optionsDialog.isHidden())
		optionsDialog.show();
	else
		optionsDialog.hide();
}

void MainWindow::apply_hocr_settings()
{
	hocr_ocr_type_nikud		= ui.cbNikud->isChecked();
	hocr_output_with_graphics	= ui.cbGraphics->isChecked();
	hocr_brightness			= ui.sliderBrightness->value();
	scannedText->setFont( ui.textFontPreview->font() );
	scannedText->clear();
	
	QTimer::singleShot( 0, this, SLOT(doOCR()));
}

void MainWindow::saveStatus()
{
	QSettings settings;
	settings.setValue( "main/image"		, imageFilename );
	settings.setValue( "main/size"		, size() );
	settings.setValue( "main/zoom"		, imageLabel->getZoomFactor() );
	settings.setValue( "main/bestfit"	, imageLabel->getBestFit() );
	settings.setValue( "main/position"	, pos() );
	settings.setValue( "main/state"		, saveState(0) );
	settings.setValue( "main/font"		, scannedText->font().toString() );
	settings.setValue( "hocr-type/nikud"	, hocr_ocr_type_nikud );
	settings.setValue( "hocr-output/graphics", hocr_output_with_graphics );
	settings.setValue( "hocr/brightness"	, hocr_brightness );
	
	settings.sync();
}

void MainWindow::loadStatus()
{
	QSettings settings;
	
	move( settings.value("main/position", QPoint(200, 200)).toPoint() );
	resize( settings.value("main/size", QSize(400, 400)).toSize() );
	restoreState( settings.value("main/state").toByteArray() );
	scannedText->setFont( QFont(settings.value("main/font").toString()) );

	hocr_ocr_type_nikud         = settings.value( "hocr-type/nikud", true  ).toBool();
	hocr_output_with_graphics   = settings.value( "hocr-output/graphics", false ).toBool();
	hocr_brightness             = settings.value( "hocr/brightness", 100).toInt();

	bool bestFit = settings.value("main/bestfit", true).toBool();
	actionBestFit->setChecked( bestFit );
	actionZoomIn ->setEnabled( !bestFit );
	actionZoomOut->setEnabled( !bestFit );
	actionZoomNormal->setEnabled( !bestFit );
	imageLabel->setBestFit( bestFit );
	imageLabel->setZoomFactor( settings.value("main/zoom", "1").toDouble() );
		
	viewImage( settings.value( "main/image" ).toString() );

	QString fontName = settings.value("main/font").toString();
	if (fontName.isEmpty())
		fontName = ui.textFontPreview->font().toString();
	QFont font;
	font.fromString( fontName );
	
	ui.textFontPreview->setFont( font );
	ui.textFontPreview->setText( font.family() );
	scannedText->setFont( font );
}

void MainWindow::closeEvent(QCloseEvent *)
{
	optionsDialog.hide();
	saveStatus();
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
	QFile file( ":/src/default.html" );
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		// no way we get into this code... but still...
		QMessageBox::critical( this, "Error", "Something could not read the built in template for the HTML document" );
		return false;
	}
	QString html = file.readAll();
	
	file.setFileName( fileName );
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical( this, "Error", "Cound not save file" );
		return false;
	}

	html = QString(html).arg("File created by QHOCR - a Qt4 GUI for HOCR").arg( text );
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
	hocr_pix->command |= HOCR_COMMAND_DICT;
	
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
//	this code calls the hocr functions in a separated thread.
//	another timer is executed every 50 milli-seconds, and will
//	update the GUI with the status of the text recognition.
//	
//	that timer, will query the thread and if it stopped (the call to
//	hocr_do_ocr() has ended) the thread object will be deleted.
	scannedText->clear();
	hocr_thread = new HOCRThread( hocr_pix, hocr_text );
	hocr_thread->start();
	hocr_timer = startTimer( 50 );
#else
//	this code calls the hocr code in a synchronous mode. this will
//	block the execution of the main thread, and will make the GUI
//	non responsive for several seconds. 
//	it's not used now, but it's left as reference
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

void MainWindow::timerEvent(QTimerEvent *)
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
}
