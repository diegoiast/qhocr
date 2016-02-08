TARGET = qhocr-cli
TEMPLATE = 
QT = core gui
LIBS += -lhebocr -L../hebocr

INCLUDEPATH += . ../hebocr/src/
SOURCES =  \
	qhocr-cli.cpp \
	qhocrthread.cpp

HEADERS += qhocrthread.h

