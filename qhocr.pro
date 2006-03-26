TMP_DIR      = .tmp
UI_DIR       = .tmp
MOC_DIR      = .tmp
OBJECTS_DIR  = .tmp

TEMPLATE	= app
CONFIG	+= qt release
DESTDIR	= bin
TARGET	= qhocr
RESOURCES	= qhocr.qrc

INCLUDEPATH	= src src/hocr

SOURCES	= src/hocr/font_layout.c \
	src/hocr/font_recognition.c \
	src/hocr/hocr.c \
	src/hocr/hocr_object.c \
	src/hocr/hocr_pixbuf.c \
	src/hocr/hocr_textbuffer.c \
	src/hocr/page_layout.c \
	src/main.cpp \
	src/mainwindow.cpp \
	src/pixmapviewer.cpp

FORMS	= src/hocr_options.ui
HEADERS	= src/mainwindow.h 	src/pixmapviewer.h

