TARGET = qhocr-cli
TEMPLATE = app
QT = core gui
CONFIG += debug
DESTDIR = bin
TMP_DIR = .tmp
UI_DIR = .tmp
MOC_DIR = .tmp
OBJECTS_DIR = .tmp
INCLUDEPATH +=   src src/hebocr/src
SOURCES =  \
	src/hebocr/src/hocr.c \
	src/hebocr/src/ho_array.c \
	src/hebocr/src/ho_array_hist.c \
	src/hebocr/src/ho_array_stat.c \
	src/hebocr/src/ho_bitmap.c \
	src/hebocr/src/ho_bitmap_hist.c \
	src/hebocr/src/ho_dimentions.c \
	src/hebocr/src/ho_font.c \
	src/hebocr/src/ho_layout.c \
	src/hebocr/src/ho_linguistics.c \
	src/hebocr/src/ho_objmap.c \
	src/hebocr/src/ho_pixbuf.c \
	src/hebocr/src/ho_recognize.c \
	src/hebocr/src/ho_recognize_nikud.c \
	src/hebocr/src/ho_segment.c \
	src/hebocr/src/ho_string.c \
	src/hebocr/src/fonts/ho_recognize_font_1.c \
	src/hebocr/src/fonts/ho_recognize_font_2.c \
    src/qhocr-cli.cpp \
    src/qhocrthread.cpp

HEADERS += src/qhocrthread.h

