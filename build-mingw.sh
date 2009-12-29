#! /bin/sh

set -x
set -e

# clean up old shite
qmake-qt4 qhocr.pro
make distclean

# start building
QMAKESPEC=fedora-win32-cross qmake-qt4 QT_LIBINFIX=4 qhocr.pro
#qmake-qt4 -spec fedora-win32-cross
make


DLLS=`i686-pc-mingw32-objdump -p bin/qhocr.exe  | grep dll | awk '{print $3}'`
for i in $DLLS mingwm10.dll ; do 
	f=/usr/i686-pc-mingw32/sys-root/mingw/bin/$i
	if [ ! -f $f ]; then continue; fi
	cp -av $f bin 
done

mkdir -p qhocr-win32
mv bin/*.exe qhocr-win32
mv bin/*.dll qhocr-win32
zip -r qhocr-win32.zip qhocr-win32
