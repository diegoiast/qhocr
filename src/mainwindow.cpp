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

#include "mainwindow.h"
#include "ui_hocr_options.h"

#define TITLE "QHOCR 0.10.16-svn"
#define MESSAGE_TIME 5000


MainWindow::MainWindow( QWidget *parent ):QMainWindow( parent )
{
	setupUi( this );
	connect( actionAboutQt   , SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	imageViewer = new PixmapViewer;
	scrollArea = new QScrollArea;
	scrollArea->setBackgroundRole(QPalette::Dark);
	scrollArea->setWidget(imageViewer);
	scrollArea->setWidgetResizable( true );
//	scrollArea->setLayoutDirection( Qt::RightToLeft );
	setCentralWidget(scrollArea);
	
	scannedText->setLayoutDirection( Qt::RightToLeft );

	statusBar()->showMessage( tr("Welcome - load an image to start"), MESSAGE_TIME );

	// non-modal options dialog
	ui.setupUi(&optionsDialog);
	connect(ui.btnTextFont, SIGNAL(clicked()), this, SLOT(_on_changeFont_clicked()) );
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(apply_hocr_settings()));

	setWindowTitle( TITLE );
	hocr_timer = 0;
	loadStatus();
	
	qhocrThread = NULL;

	// I don't really like this code, but I have no other alternative, read:
	// http://www.qtcentre.org/forum/f-qt-programming-2/t-qfiledialoggetsavefilename-default-extension-8503.html
	extByMessage[ tr("Text files (*.txt *.utf8)") ] = ".txt";
	extByMessage[ tr("HTML Files (*.html *.htm)") ] = ".html";
	
	QHashIterator<QString, QString> i(extByMessage);
 	while (i.hasNext()) 
 	{
 		i.next();
 		saveMessage += i.key();
 		if (i.hasNext()) 
 			saveMessage += ";;";
	}
}

void MainWindow::on_actionAbout_triggered()
{
	QMessageBox::information( 0,
		tr("About QHOCR 0.10.16"), 
		tr("QHOCR - a Qt4 GUI front end to the HOCR library"
		"<br>Diego Iastrubni &lt;<a href='elcuco@kde.org'>elcuco@kde.org</a>&gt; 2005,2006,2009"
		"<br><br>This application is free software, released under the terms of GPL"
		"read LICENSE.GPL for more intormation. Newer versions can be found at"
		" <a href='http://code.google.com/p/qhocr/'>http://code.google.com/p/qhocr/</a> <br>"
		"<br>This application uses <b>libhocr 0.10.16</b> by "
		"Kobi Zamir &lt;<a href='kzamir@walla.co.il'>kzamir@walla.co.il</a>&gt;, "
		"which can be found at <a href='http://hocr.berlios.de'>http://hocr.berlios.de</a>")
	);
}

void MainWindow::on_action_Quit_triggered()
{
	QApplication::exit(0);
}

void MainWindow::on_action_Open_triggered()
{
	statusBar()->showMessage( tr("Loading file") );
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
		statusBar()->showMessage( tr("openning aborted"), MESSAGE_TIME );
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
		tr("Choose a file to save to"),
		lastDir,
		saveMessage,
		&sf
	);

	if (s.isEmpty())
		return;
	
	bool status;
	
	// what if the users did not specify a suffix...?
	QFileInfo f( s );
	if (f.suffix().isEmpty())
	{
		// http://www.qtcentre.org/forum/f-qt-programming-2/t-qfiledialoggetsavefilename-default-extension-8503.html
		s += extByMessage[sf];
	}

	QString s_lower = s.toLower();
	
	if (s_lower.endsWith(".html") || s_lower.endsWith(".htm"))
		status = saveHTML( s, scannedText->toPlainText() );
	else if (s_lower.endsWith(".utf8"))
		status = saveText( s, scannedText->toPlainText(), true );
	else
		status = saveText( s, scannedText->toPlainText(), false );

	if (status)
	{
		statusBar()->showMessage( tr("Text saved"), MESSAGE_TIME );
		lastSave = s;
	}
	else
		statusBar()->showMessage( tr("An error occured while saving the text"), MESSAGE_TIME );
}

void MainWindow::on_actionZoomIn_triggered()
{
	imageViewer->zoomBy( 1.1 );
}

void MainWindow::on_actionZoomOut_triggered()
{
	imageViewer->zoomBy( 1 / 1.1 );
}

void MainWindow::on_actionZoomNormal_triggered()
{
	imageViewer->zoomBy( 0 );
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
	bool b = imageViewer->getBestFit();
	imageViewer->setBestFit( !b );

	actionZoomNormal->setEnabled( b );
	actionZoomIn->setEnabled( b );
	actionZoomOut->setEnabled( b );
	actionZoomNormal->setEnabled( b );
}

void MainWindow::on_actionHOCR_Preferences_triggered()
{
//	ui.cbNikud->setChecked(hocr_ocr_type_nikud);

	if (optionsDialog.isHidden())
		optionsDialog.show();
	else
		optionsDialog.hide();
}

void MainWindow::apply_hocr_settings()
{
//	hocr_ocr_type_nikud		= ui.cbNikud->isChecked();
//	scannedText->setFont( ui.textFontPreview->font() );
	scannedText->clear();
	
	QTimer::singleShot( 0, this, SLOT(doOCR()));
}

void MainWindow::saveStatus()
{
	QSettings settings;
	settings.setValue( "main/image"		, imageFilename );
	settings.setValue( "main/size"		, size() );
	settings.setValue( "main/zoom"		, imageViewer->getZoomFactor() );
	settings.setValue( "main/bestfit"	, imageViewer->getBestFit() );
	settings.setValue( "main/position"	, pos() );
	settings.setValue( "main/state"		, saveState(0) );
	settings.setValue( "main/font"		, scannedText->font().toString() );
//	settings.setValue( "hocr-type/nikud"	, hocr_ocr_type_nikud );
//	settings.setValue( "hocr-output/graphics", hocr_output_with_graphics );
//	settings.setValue( "hocr/brightness"	, hocr_brightness );
	
	settings.sync();
}

void MainWindow::loadStatus()
{
	QSettings settings;
	
	move( settings.value("main/position", QPoint(200, 200)).toPoint() );
	resize( settings.value("main/size", QSize(400, 400)).toSize() );
	restoreState( settings.value("main/state").toByteArray() );
	scannedText->setFont( QFont(settings.value("main/font").toString()) );

//	hocr_ocr_type_nikud         = settings.value( "hocr-type/nikud", true  ).toBool();
//	hocr_output_with_graphics   = settings.value( "hocr-output/graphics", false ).toBool();
//	hocr_brightness             = settings.value( "hocr/brightness", 100).toInt();

	QString fontName = settings.value("main/font").toString();
	if (fontName.isEmpty())
		fontName = ui.textFontPreview->font().toString();
	QFont font;
	font.fromString( fontName );
	
	ui.textFontPreview->setFont( font );
	ui.textFontPreview->setText( font.family() );
	scannedText->setFont( font );

	bool bestFit = settings.value("main/bestfit", true).toBool();
	actionBestFit->setChecked( bestFit );
	actionZoomIn ->setEnabled( !bestFit );
	actionZoomOut->setEnabled( !bestFit );
	actionZoomNormal->setEnabled( !bestFit );
	imageViewer->setBestFit( bestFit );
	imageViewer->setZoomFactor( settings.value("main/zoom", "1").toDouble() );
	//imageViewer->loadImage( settings.value( "main/image" ).toString() );
	viewImage( settings.value( "main/image" ).toString() );
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
	
	imageFilename = fileName;
	imageToOCR.load( fileName );
	imageViewer->setImage( imageToOCR );
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
		QMessageBox::critical( this, 
			tr("Error"), 
			tr("Something could not read the built in template for the HTML document") 
		);
		return false;
	}
	QString html = file.readAll();
	
	file.setFileName( fileName );
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QMessageBox::critical( this, tr("Error"), tr("Cound not save file") );
		return false;
	}

	html = QString(html).arg( tr("File created by QHOCR - a Qt4 GUI for HOCR") ).arg( text );
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
		QMessageBox::critical( this, tr("Error"), tr("Cound not save file") );
		return false;
	}

	QTextStream out(&file);
	out.setCodec( QTextCodec::codecForName(unicode?"UTF-8":"Windows 1255") );
	out << text;
	return true;
}

void MainWindow::doOCR()
{
	if (hocr_timer != 0)
	{
		killTimer(hocr_timer);
		hocr_timer = 0;
	}
	
	hocr_timer = startTimer( 50 );
	if (!qhocrThread){
		qhocrThread = new QHOCRThread;
		connect( qhocrThread, SIGNAL(finished()), this, SLOT(ocrEnded()) );
		connect( qhocrThread, SIGNAL(stageChanged(HOCR_STAGES::stageNames)), this, SLOT(OCR_changedState(HOCR_STAGES::stageNames)));
	}
	
	if (qhocrThread->isRunning())
		return;
	
	qhocrThread->setImage( imageToOCR );
	qhocrThread->start();
	
	scannedText->clear();
}

void MainWindow::ocrEnded()
{
	if (!qhocrThread)
		return;
	
//	scannedText->setPlainText( qhocrThread->getString() );
	scannedText->setHtml(
		QString("<div dir='rtl' style='text-align:left' >%1</div>").arg(qhocrThread->getString()) 
	);
	
	delete qhocrThread;
	qhocrThread = NULL;
	
	statusBar()->showMessage( tr("OCR ended"), MESSAGE_TIME );
}

void MainWindow::OCR_changedState(HOCR_STAGES::stageNames newStage)
{
	switch (newStage)
	{
		case HOCR_STAGES::idle:
			// phase 0 - doing nothing
			break;

		case HOCR_STAGES::imagePreProces:
			// phase 1 - image pre-processing
			break;

		case HOCR_STAGES::layoutAnalysis:
			// phase 2 - layout analysis
			break;

		case HOCR_STAGES::fontRecognition:
			// phase 3 - font recognition
			break;
	}
}

void MainWindow::timerEvent(QTimerEvent *)
{
	if (!qhocrThread)
		return;
	
	int percentage = qhocrThread->getProcess();
	switch (qhocrThread->getStage())
	{
		case HOCR_STAGES::idle:
			// phase 0 - doing nothing
			break;
			
		case HOCR_STAGES::imagePreProces:
			// phase 1 - image pre-processing
			statusBar()->showMessage( tr("Processing image: %1% - 1/3").arg(percentage), MESSAGE_TIME );
			break;
			
		case HOCR_STAGES::layoutAnalysis:
			// phase 2 - layout analysis
			statusBar()->showMessage( tr("Layout analysis: %1% - 2/3").arg(percentage), MESSAGE_TIME );
			break;
			
		case HOCR_STAGES::fontRecognition:
			// phase 3 - font recognition
			statusBar()->showMessage( tr("Font recognition: %1% - 3/3").arg(percentage), MESSAGE_TIME );
			break;
	}
}
