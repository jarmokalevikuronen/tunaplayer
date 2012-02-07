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

#ifndef TPPATHUTILS_H
#define TPPATHUTILS_H

#include <QString>
#include <QStringList>
#include <QDir>

//! @class TPPathUtils
//! @brief Common place to ask all path related information
class TPPathUtils
{
private:

    static QString ensureBasePath(QString subdir = QString());

public:
    //! @brief Returns the name of the track database
    //! TODO: Change name to getTrackDbFilename().
    static QString getMusicDbFilename();
    static QString getDbItemTempFilename(const QString dbId, const QString id);
    static QString getFeedsDbFilename();
    static QString getArtistDbFilename();
    static QString getAlbumDbFilename();
    static QString getPlaylistFolder();
    static QString getPlaylistFolderRo();
    static QString getPlaylistArtFolder();
    static QString getPlaylistArtFolderRo();
    static QString getAlbumArtFolder();
    static QString getMusicScannerRootPath();
    static QString getAlbumArtDownloadFolder();
    static QString getWebServerRootFolder();
    static QString getWebServerRootFolderRo();
    static QString getNormalizedPathForWebServer(const QString fullPath);
    static QString getDefaultPlaylistFilename();
    static QString getPlaylistDbFilename();
    static QString getSettingsFilename();
    static QString getFeedsSettingsDbFilename();
    static QString getFeedItemsSettingsDbFilename();
    static QString getTspFolder();
    static QStringList getMediaPaths();
    static QString getRootPath();
    static QString getArtistNameMappingConfigFile();
};

#endif // TPPATHUTILS_H
