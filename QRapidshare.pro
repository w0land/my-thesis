TEMPLATE = app
QT = gui core network
CONFIG += qt warn_on console debug
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/dialog.ui
HEADERS = src/dialogimpl.h src/qrapidsharedownload.h src/debugutils.h src/qrapidshareuser.h
SOURCES = src/dialogimpl.cpp src/main.cpp src/qrapidsharedownload.cpp src/debugutils.cpp src/qrapidshareuser.cpp
CONFIG -= release

LIBS += -L/usr/local/lib/
SOURCES -= log/rslog.cpp

HEADERS -= log/rslog.h

