TARGET = qhocr-cli
TEMPLATE = app
QT = debug core \
    gui
DESTDIR = bin
TMP_DIR = .tmp
UI_DIR = .tmp
MOC_DIR = .tmp
OBJECTS_DIR = .tmp
INCLUDEPATH += src/libhocr-0.10.16/src/
SOURCES = src/libhocr-0.10.16/src/hocr.c \
    src/libhocr-0.10.16/src/ho_array.c \
    src/libhocr-0.10.16/src/ho_array_hist.c \
    src/libhocr-0.10.16/src/ho_array_stat.c \
    src/libhocr-0.10.16/src/ho_bitmap.c \
    src/libhocr-0.10.16/src/ho_bitmap_hist.c \
    src/libhocr-0.10.16/src/ho_dimentions.c \
    src/libhocr-0.10.16/src/ho_font.c \
    src/libhocr-0.10.16/src/ho_layout.c \
    src/libhocr-0.10.16/src/ho_linguistics.c \
    src/libhocr-0.10.16/src/ho_objmap.c \
    src/libhocr-0.10.16/src/ho_pixbuf.c \
    src/libhocr-0.10.16/src/ho_recognize.c \
    src/libhocr-0.10.16/src/ho_recognize_nikud.c \
    src/libhocr-0.10.16/src/ho_segment.c \
    src/libhocr-0.10.16/src/ho_string.c \
    src/libhocr-0.10.16/src/fonts/ho_recognize_font_1.c \
    src/libhocr-0.10.16/src/fonts/ho_recognize_font_2.c \
    src/libhocr-0.10.16/src/fonts/ho_recognize_font_3.c \
    src/libhocr-0.10.16/src/fonts/ho_recognize_font_4.c \
    src/libhocr-0.10.16/src/fonts/ho_recognize_font_5.c \
    src/libhocr-0.10.16/src/fonts/ho_recognize_font_6.c \
    src/qhocr-cli.cpp \
    src/qhocrthread.cpp

HEADERS += src/qhocrthread.h

# Make hocr happy
DEFINES += PACKAGE_NAME=\\\"qhocr\\\" \
    PACKAGE_TARNAME=\\\"qhocr\\\" \
    PACKAGE_VERSION=\\\"0.10.16\\\" \
    PACKAGE_STRING=\\\"qhocr_0.10.16\\\" \
    PACKAGE_BUGREPORT=\\\"Diego_Iastrubni_\<diegoiast\@gmail.com\>\\\" \
    PACKAGE=\\\"libhocr\\\" \
    VERSION=\\\"0.10.16\\\" \
    BUILD=\\\"qt4\\\"
