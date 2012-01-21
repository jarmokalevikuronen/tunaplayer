/*
This file is part of tunaplayer project
Copyright (C) 2012  Jarmo Kuronen <jarmok@iki.fi>

This is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this software; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "tppathutils.h"
#include "tputils.h"
#include "tpclargs.h"


#define TP_DATA_FOLDER                  ".tunaplayer"
#define TP_MUSIC_DB_FILE                "track.db"
#define TP_MUSIC_DB_TEMP_FOLDER         TP_MUSIC_DB_FILE "-tmp"
#define TP_ARTIST_DB_FILE               "artist.db"
#define TP_ALBUM_DB_FILE                "album.db"
#define TP_FEED_DB_FILE                 "feedlist.txt"
#define TP_PLAYLIST_DB_FILE             "playlist.db"
#define TP_SETTINGS_FILE                "settings.ini"
#define TP_FEED_SETTINGS_DB_FILE        "feeddata.db"
#define TP_FEEDITEM_SETTINGS_DB_FILE    "feeditemdata.db"

QString TPPathUtils::ensureBasePath(QString subdir)
{
    QString home = QDir::homePath();
    Q_ASSERT(home.length());

    home += QDir::separator();
    home += TP_DATA_FOLDER;
    home += QDir::separator();

    if (subdir.length())
    {
        home += subdir;
        home += QDir::separator();
    }

    QDir creator;
    Q_ASSERT(creator.mkpath(home));
    return home;
}

QString TPPathUtils::getMusicDbFilename()
{
    QString home = ensureBasePath("db");

    home += TP_MUSIC_DB_FILE;

    return home;
}

QString TPPathUtils::getDbItemTempFilename(const QString dbId, const QString itemId)
{
    ensureBasePath("db");

    QString fp = QString("db") + QDir::separator();
    fp += TPUtils::ensureFilenameCharacters(dbId);
    fp += QString("-tmp-items");
    fp = ensureBasePath(fp);
    fp += TPUtils::ensureFilenameCharacters(itemId);

    return fp;
}


QString TPPathUtils::getFeedsDbFilename()
{
    QString home = ensureBasePath("feeds");
    home += TP_FEED_DB_FILE;
    return home;
}

QString TPPathUtils::getFeedsSettingsDbFilename()
{
    QString home = ensureBasePath("feeds");
    home += TP_FEED_SETTINGS_DB_FILE;
    return home;
}

QString TPPathUtils::getFeedItemsSettingsDbFilename()
{
    QString home = ensureBasePath("feeds");
    home += TP_FEEDITEM_SETTINGS_DB_FILE;
    return home;
}

QString TPPathUtils::getPlaylistFolder()
{
    return ensureBasePath("playlists");
}

QString TPPathUtils::getPlaylistArtFolder()
{
    (void)getPlaylistFolder();
    QString subdir = QString("playlists") + QDir::separator() + QString("icons");

    return ensureBasePath(subdir);
}

QString TPPathUtils::getRootPath()
{
    return ensureBasePath("");
}

QString TPPathUtils::getAlbumArtFolder()
{
    return ensureBasePath("albumart");
}

QString TPPathUtils::getMusicScannerRootPath()
{
    return QDir::homePath();
}

QString TPPathUtils::getAlbumArtDownloadFolder()
{
    ensureBasePath("albumart");
    QString subdir = QString("albumart") + QDir::separator() + QString("download");
    return ensureBasePath(subdir);
}

QString TPPathUtils::getArtistDbFilename()
{
    QString path = ensureBasePath("db");
    path.append(TP_ARTIST_DB_FILE);
    return path;
}

QString TPPathUtils::getAlbumDbFilename()
{
    QString path = ensureBasePath("db");
    path += TP_ALBUM_DB_FILE;
    return path;
}

QString TPPathUtils::getWebServerRootFolder()
{
    return ensureBasePath();
}

QString TPPathUtils::getNormalizedPathForWebServer(const QString fullPathToObject)
{
    QString webServerRootPath = getWebServerRootFolder();

    if (fullPathToObject.startsWith(webServerRootPath))
        return fullPathToObject.mid(webServerRootPath.length());

    return QString("");
}

QString TPPathUtils::getDefaultPlaylistFilename()
{
    QString path = getPlaylistFolder();
    path += "autogen-playlist.m3u";

    return path;
}

QString TPPathUtils::getPlaylistDbFilename()
{
    QString path = ensureBasePath("db");
    path.append(TP_PLAYLIST_DB_FILE);
    return path;
}

QString TPPathUtils::getSettingsFilename()
{
    QString path = ensureBasePath();
    path += TP_SETTINGS_FILE;
    return path;
}

QString TPPathUtils::getTspFolder()
{
    return ensureBasePath("tsp");
}


QStringList TPPathUtils::getMediaPaths()
{
    QStringList listOfPaths;
    QDir d;

    // From command line, multiple paths can be gived if separated with ';'
    // E.g. /home/jarmo/;/home/jokumuu
    listOfPaths << TPCLArgs::instance().arg(TPCLArgs::cliArgMediaPath, d.homePath()).toString().split(';');
    return listOfPaths;
}
