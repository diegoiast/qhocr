TARGET = qhocr-cli
TEMPLATE = 
QT = core gui
LIBS += -lhebocr -L .

INCLUDEPATH += . ../hebocr/src/
SOURCES =  \
	qhocr-cli.cpp \
	qhocrthread.cpp

HEADERS += qhocrthread.h

