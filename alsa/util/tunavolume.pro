CONFIG += console
CONFIG -= app_bundle
CONFIG += warn_on

# ALSA
LIBS += -L/usr/lib/ -lasound

TEMPLATE = app

SOURCES += tunavolume.cpp

target = tunavolume
target.path = $$(DESTDIR)/usr/bin

INSTALLS += target
