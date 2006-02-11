TMP_DIR      = .tmp
UI_DIR       = .tmp
MOC_DIR      = .tmp
OBJECTS_DIR  = .tmp

#CONFIG      = DEBUG
CONFIG		 += qt release
TARGET       = qhocr
DESTDIR      = bin 
TEMPLATE     = app

RESOURCES   += qhocr.qrc
INCLUDEPATH += src src/hocr


FORMS        = src/hocr_options.ui

HEADERS	+= \
	src/mainwindow.h \
	src/pixmapviewer.h \

SOURCES	+= \
	src/hocr/font_layout.c \
	src/hocr/font_recognition.c \
	src/hocr/hocr.c \
	src/hocr/hocr_object.c \
	src/hocr/hocr_pixbuf.c \
	src/hocr/hocr_textbuffer.c \
	src/hocr/page_layout.c \
	src/main.cpp \
	src/mainwindow.cpp \
	src/pixmapviewer.cpp 
	
# 	src/hocr/font_layout.c \
# 	src/hocr/font_markers.c \
# 	src/hocr/font_recognition.c \
# 	src/hocr/hocr.c \
# 	src/hocr/hocr_object.c \
# 	src/hocr/hocr_pixbuf.c \
# 	src/hocr/hocr_textbuffer.c \
# 	src/hocr/page_layout.c \
# 	src/main.cpp \
# 	src/mainwindow.cpp \
# 	src/pixmapviewer.cpp \
