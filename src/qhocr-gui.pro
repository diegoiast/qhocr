TEMPLATE	=   app
TARGET		=   qhocr
CONFIG		+=   qt release

VERSION = 0.12
QT += widgets

INCLUDEPATH +=   . ../hebocr/src/

RESOURCES += ./qhocr.qrc 
LIBS += -lhebocr -L../hebocr

FORMS += \
	hocr_options.ui \
	qhocr_mainwin.ui \
	imageoptionscontrol.ui \

SOURCES +=    \
	qhocrthread.cpp \
	mainwindow.cpp \
	pixmapviewer.cpp \
	logger.cpp \
	main.cpp \
	imageoptionscontrol.cpp

HEADERS +=   \
	qhocrthread.h \
	mainwindow.h \
	pixmapviewer.h \
	logger.h \
	imageoptionscontrol.h

OTHER_FILES += \
	../INSTALL \
	../COPYING.gpl \
	../COPYING.bsd \
	../AUTHORS \
	../COPYING \
	../ChangeLog \
	images/zoomout.png \
	images/zoomin.png \
	images/save.png \
	images/paste.png \
	images/open.png \
	images/new.png \
	images/cut.png \
	images/copy.png
