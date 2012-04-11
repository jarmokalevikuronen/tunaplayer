# -------------------------------------------------
# Project created by QtCreator 2011-03-22T22:27:20
# -------------------------------------------------

CONFIG += console
CONFIG -= app_bundle
CONFIG += warn_on
INCLUDEPATH += ./youtube ./search/sort ./playlist ./feed ./webserver ./webserver/libwebsocket ./albumart/util ./albumart/fetch ./basictypes ./search ./alsa ./db

# TAGLIB
LIBS += -L/usr/lib/ -ltag

QT += gui
QT += network
QT += xml
TEMPLATE = app

##### BEGIN PLAYER
DEPENDPATH += ./player
SOURCES += player.cpp \
    tpreferencecounted.cpp \
    playerengine.cpp \
    musicplayercore.cpp \
    feed/tpfeedmgr.cpp \
    feed/tpfeed.cpp \
    feed/tpfeeddownloader.cpp \
    db/tpassociativedbitem.cpp \
    db/tpassociativedb.cpp \
    db/tpassociativeobject.cpp \
    tpmediafilemetaextractor.cpp \
    tpsettings.cpp \
    tppathutils.cpp \
    albumart/fetch/tplastfmimagesearch.cpp \
    albumart/fetch/googleimagesearch.cpp \
    albumart/fetch/albumartdownloadrequest.cpp \
    albumart/fetch/albumartdownloader.cpp \
    albumart/util/albumartutil.cpp \
    tputils.cpp \
    playlist/tpplaylistrunner.cpp \
    tpidbase.cpp \
    search/tpstoredprocedureargs.cpp \
    search/tpsearchfilterevalargs.cpp \
    search/operations/tpsearchfilteropand.cpp \
    search/operations/tpsearchfilteropor.cpp \
    search/operations/tpsearchfilteropnand.cpp \
    search/operations/tpsearchfilteropcontainerbase.cpp \
    search/operations/tpsearchfilteropmatchbase.cpp \
    search/operations/tpsearchfiltermatchcontains.cpp \
    search/operations/tpsearchfiltermatchequals.cpp \
    search/operations/tpsearchfiltermatchstartswith.cpp \
    search/operations/tpsearchfiltermatchendswith.cpp \
    search/operations/tpsearchfiltermatchcomparenum.cpp \
    search/operations/tpsearchfilteropfactory.cpp \
    search/tpstoredprocedure.cpp \
    albumart/fetch/tpautomaticalbumartdownloader.cpp \
    search/tpsearchfacadedataproviderinterface.cpp \
    search/tpsearchfacadedataproviders.cpp \
    search/tpsearchobjectprovider.cpp \
    search/tpsearchresults.cpp \
    search/tpstoreprocedurerunner.cpp \
    db/tptrackdb.cpp \
    db/tpartistdb.cpp \
    db/tpalbumdb.cpp \
    db/tpdatabases.cpp \
    tpplaybackstatisticscollector.cpp \
    search/tpstoredproceduremgr.cpp \
    basictypes/tpartist.cpp \
    tpaudioutils.cpp \
    tpfilescanner.cpp \
    basictypes/tpalbum.cpp \
    tpgenre.cpp \
    basictypes/tptrack.cpp \
    tpclargs.cpp \
    alsa/tpalsavolume.cpp \
    tpsignalhandler.cpp \
    tplog.cpp \
    webserver/tpwebsocketipaddressmask.cpp \
    search/sort/tpsortinterfacefactory.cpp \
    search/sort/tpsortableassociativeobject.cpp \
    search/sort/tpsortimplementationnumbase.cpp \
    search/sort/tpsortimplementationstringbase.cpp \
    search/sort/tpsortimplementationstring.cpp \
    search/sort/tpsortimplementationnum.cpp \
    tpclientconnectionmonitor.cpp \
    webserver/tpwebsocketprotocoleventfilter.cpp \
    tpusermanager.cpp \
    youtube/tpyoutubesearch.cpp \
    youtube/tpyoutubedb.cpp \
    youtube/tpyoutubeobject.cpp \
    basictypes/tpobjectdelegate.cpp

HEADERS += player.h \
    tpreferencecounted.h \
    playerengine.h \
    musicplayercore.h \
    feed/tpfeedmgr.h \
    feed/tpfeed.h \
    feed/tpfeeddownloader.h \
    db/tpassociative.h \
    db/tpassociativedbitem.h \
    db/tpassociativedb.h \
    db/tpassociativeobject.h \
    tpassociativemeta.h \
    tpmediafilemetaextractor.h \
    tpsettings.h \
    tppathutils.h \
    albumart/fetch/tplastfmimagesearch.h \
    albumart/fetch/googleimagesearch.h \
    albumart/fetch/albumartdownloadrequest.h \
    albumart/fetch/albumartdownloader.h \
    albumart/util/albumartutil.h \
    tputils.h \
    playlist/tpplaylistrunner.h \
    tpidbase.h \
    tpidbasemap.h \
    tpschemes.h \
    search/tpstoredprocedureargs.h \
    search/operations/tpsearchfilteropinterface.h \
    search/tpsearchfilterevalargs.h \
    search/operations/tpsearchfilteropand.h \
    search/operations/tpsearchfilteropor.h \
    search/operations/tpsearchfilteropnand.h \
    search/operations/tpsearchfilteropcontainerbase.h \
    search/operations/tpsearchfilteropmatchbase.h \
    search/operations/tpsearchfiltermatchcontains.h \
    search/operations/tpsearchfiltermatchequals.h \
    search/operations/tpsearchfiltermatchstartswith.h \
    search/operations/tpsearchfiltermatchendswith.h \
    search/operations/tpsearchfiltermatchcomparenum.h \
    search/operations/tpsearchfilteropfactory.h \
    search/tpstoredprocedure.h \
    albumart/fetch/tpautomaticalbumartdownloader.h \
    search/tpsearchfacadedataproviderinterface.h \
    search/tpsearchfacadedataproviders.h \
    search/tpsearchobjectprovider.h \
    search/tpsearchresults.h \
    search/tpstoreprocedurerunner.h \
    db/tpdbtemplate.h \
    db/tptrackdb.h \
    db/tpartistdb.h \
    db/tpalbumdb.h \
    db/tpdatabases.h \
    musicplaybackdefines.h \
    tpplaybackstatisticscollector.h \
    search/tpstoredproceduremgr.h \
    basictypes/tpartist.h \
    tpaudioutils.h \
    tpfilescanner.h \
    basictypes/tpalbum.h \
    tpgenre.h \
    basictypes/tptrack.h \
    webserver/tpprotocoldefines.h \
    tplog.h \
    tpclargs.h \
    alsa/tpalsavolume.h \
    tpsignalhandler.h \
    webserver/tpwebsocketipaddressmask.h \
    search/sort/tpsortinterface.h \
    search/sort/tpsortinterfacefactory.h \
    search/sort/tpsortableassociativeobject.h \
    search/sort/tpsortimplementationnumbase.h \
    search/sort/tpsortimplementationstringbase.h \
    search/sort/tpsortimplementationstring.h \
    search/sort/tpsortimplementationnum.h \
    search/sort/tpsort.h \
    basictypes/tpcachedvalues.h \
    tpclientconnectionmonitor.h \
    webserver/tpwebsocketprotocoleventfilter.h \
    tpusermanager.h \
    youtube/tpyoutubesearch.h \
    youtube/tpyoutubedb.h \
    youtube/tpyoutubeobject.h \
    basictypes/tpobjectdelegate.h
##### END PLAYER

##### BEGIN JSON
DEPENDPATH += ./3rdparty/json
SOURCES += ./3rdparty/json/json.cpp
HEADERS += ./3rdparty/json/json.h
INCLUDEPATH += ./3rdparty/json
#### END JSON

##### BEGIN TAGLIB
INCLUDEPATH += /usr/include/taglib
##### END TAGLIB

##### BEGIN LIBWEBSOCKET
DEPENDPATH += ./3rdparty/libwebsocket
SOURCES += ./3rdparty/libwebsocket/sha-1.c \
    ./3rdparty/libwebsocket/parsers.c \
    ./3rdparty/libwebsocket/md5.c \
    ./3rdparty/libwebsocket/libwebsockets.c \
    ./3rdparty/libwebsocket/handshake.c \
    ./3rdparty/libwebsocket/extension-x-google-mux.c \
    ./3rdparty/libwebsocket/extension-deflate-stream.c \
    ./3rdparty/libwebsocket/extension.c \
    ./3rdparty/libwebsocket/client-handshake.c \
    ./3rdparty/libwebsocket/base64-decode.c

HEADERS += ./3rdparty/libwebsocket/private-libwebsockets.h \
    ./3rdparty/libwebsocket/libwebsockets.h \
    ./3rdparty/libwebsocket/extension-x-google-mux.h \
    ./3rdparty/libwebsocket/extension-deflate-stream.h
##### END LIBWEBSOCKET

##### BEGIN WEBSERVER
DEPENDPATH += ./webserver
SOURCES += ./webserver/tplibwebsocketinterface.cpp \
    ./webserver/tpwebsocketvirtualfolder.cpp \
    ./webserver/tpwebsocketprotocol.cpp \
    ./webserver/tpwebsocketprotocolmessage.cpp

HEADERS += ./webserver/tplibwebsocketinterface.h \
    ./webserver/tpwebsocketdataproviderinterface.h \
    ./webserver/tpwebsocketvirtualfolder.h \
    ./webserver/tpwebsocketprotocol.h \
    ./webserver/tpwebsocketprotocolmessage.h
##### END WEBSERVER

##### BEGIN PLAYLISTS
DEPENDPATH += ./playlist

HEADERS += ./playlist/playlistutils.h \
    ./playlist/playlistmgr.h \
    ./playlist/playlist.h
SOURCES += ./playlist/playlistutils.cpp \
    ./playlist/playlistmgr.cpp \
    ./playlist/playlist.cpp
##### END PLAYLISTS

#### BEGIC BASIC TYPES and containers
DEPENDPATH += ./basictypes
#### END BASIC TYPES

SOURCES += main.cpp

target = tunaplayer
CONFIG(debug, debug|release) { 
    message("Debug build of tunaPlayer")
    CONFIG += -DDEBUG -D_DEBUG
    DESTDIR = ./tunaplayer-debug
    OBJECTS_DIR = ./tunaplayer-debug/objects
}
else { 
    message("Release build of tunaPlayer")
    DESTDIR = ./tunaplayer-release
    OBJECTS_DIR = ./tunaplayer-release/objects
}

target.path = $$PREFIX/usr/bin
sources.files = $$SOURCES $$HEADERS tunaplayer.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/tunaplayer
INSTALLS += target

#
# HTML UI
#
html.files += webui/jquery.min.js
html.files += webui/jquery.jqote2.min.js
html.files += webui/tunaplayer.css
html.files += webui/tunaplayer_n9.css
html.files += webui/tunaplayerdaemon.js
html.files += webui/tunaplayer.html
html.files += webui/tunaplayer.tmpl
html.files += webui/tunaplayer_n9.tmpl
html.files += webui/jgestures.min.js
html.path = $$PREFIX/usr/share/tunaplayer/
INSTALLS += html

#
# Tunaplayer Stored Procedures for queries
#
tsp.files += rt-environment/tsp/*
tsp.path = $$PREFIX/usr/share/tunaplayer/tsp
INSTALLS += tsp

#
# Other configuration files
#
cfg.files += rt-environment/configs/*
cfg.path = $$PREFIX/usr/share/tunaplayer/configs
INSTALLS += cfg

#
# Hardcoded playlists + their icons
#
playlist.files += rt-environment/playlists/*.m3u
playlist.path = $$PREFIX/usr/share/tunaplayer/playlists
playlist_icons.files += rt-environment/playlists/icons/*
playlist_icons.path = $$PREFIX/usr/share/tunaplayer/playlists/icons
INSTALLS += playlist
INSTALLS += playlist_icons

#
# Script files to aid playback.
#
scripts.files += scripts/*.sh
scripts.path = $$PREFIX/usr/bin/
INSTALLS += scripts


#
# Autostart
#
daemon_autostart.files += ubuntu/tunaplayer.desktop
daemon_autostart.path = $$PREFIX/etc/xdg/autostart
INSTALLS += daemon_autostart
