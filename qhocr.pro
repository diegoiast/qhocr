TEMPLATE = subdirs
SUBDIRS = libhebocr qhocr qhocr-cli

# we don't really use subdir, but sub projects
libhebocr.file = src/hebocr-lib.pro
qhocr.file  = src/qhocr-gui.pro
qhocr-cli.file  = src/qhocr-cli.pro

# the apps depend on the library
qhocr.depends = libhebocr
qhocr-cli.depends = libhebocr