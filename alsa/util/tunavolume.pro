CONFIG += console
CONFIG -= app_bundle
CONFIG += warn_on

# ALSA
LIBS += -L/usr/lib/ -lasound

TEMPLATE = app

SOURCES += tunavolume.cpp

TARGET = tunavolume
TARGET.path = /usr/bin

INSTALLS += TARGET
