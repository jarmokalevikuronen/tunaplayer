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

#ifndef MUSICPLAYERCORE_H
#define MUSICPLAYERCORE_H

#include <QObject>

#include <QStringList>
#include <QString>

#include "playlistmgr.h"
#include "playlistutils.h"
#include "playerengine.h"
#include "tpfilescanner.h"
#include "albumartdownloader.h"
#include "albumartdownloadrequest.h"
#include "feed/tpfeedmgr.h"
#include "tpautomaticalbumartdownloader.h"
#include "db/tpdatabases.h"
#include "tpplaybackstatisticscollector.h"
#include "tpstoredproceduremgr.h"
#include "tpwebsocketprotocol.h"
#include "tpsearchobjectprovider.h"
#include "tpsearchresults.h"
#include "tpalsavolume.h"

class TPMusicPlayerCore : public QObject
{
    Q_OBJECT

public:

    explicit TPMusicPlayerCore(TPWebSocketProtocol *_protocol);
    ~TPMusicPlayerCore();

    //! @brief Starts the asynchrnous processing.
    bool start();

private: // PLAYLIST FUNCTIONALITY

    //! @brief Adds an object to playlist. At this point the object
    //! can be either a
    //!  1. a Playlist -> All songs within a playlist will be added. (playlist:// scheme)
    //!  2. a Track -> Just that specific track will be added (track:// scheme)
    //!  3. a Album -> all songs within album will be added (album:// scheme)
    //!  4. a Artist -> all songs from a given artist will be put. (artist:// scheme)
    //!  5. a Complete Feed (feed:// scheme)
    //!  6. a Feed Item  (feeditem.// scheme)
    //!  7. a URL (http:// scheme)
    //! @param aObject object that holds the identifier of the object
    //! to be added. In general, this is held by the "id" role for seach model, or playlist model.
    //! @param position back/front of the playlist.
    bool addToPlaylist(QVariant aObject, const QString &position);

    //! @brief removes a specified track from the current playlist.
    bool removeFromPlaylist(const QString id);

    //! @brief Clears the currently active playlist.
    void clearActivePlaylist();

    //! @brief Changes the currently active playlist to something else
    //! specified by the idOrName parameter (can be playlist name or id, as indicated).
    bool setActivePlaylist(const QString idOrName);

    //! @brief Seeks to specified track within currently active playlist.
    bool seekToTrack(const QString trackId);

    //! mode = "queue", "once", "repeat"
    void setActivePlaylistMode(QVariant mode);

    QVariant getActivePlaylistMode();

    //! @brief Saves the current (=Player) playlist with given name
    //! but if the name is not given, and the playlist was a cloned one,
    //! the original playlist this playlist was cloned from is to be replaced accordingly.
    bool saveCurrentPlaylist(const QString name);

    bool removePlaylist(const QString id);

private slots:

    void onProtocolMessage(TPWebSocketProtocol *protocol, TPWebSocketProtocolMessage message);

public:

private: // ALBUM ART SEARCH FUNCTIONALITY

    //! @brief Searches a album art from the supported
    //! services (lastfm, google image search)
    //! @param album or track identifier, E.g. album://a12321312ab.. or track://a1232342..
    bool searchAlbumArt(const QString id);
    void cancelSearchAlbumArt();
    bool selectSearchedAlbumArt(const QString id);


    bool executePlaybackOperation(const QString operation);

signals:

    void playingTrackChanged();
    void playingAlbumChanged();
    void playbackPositionChanged(QVariant currentSeconds, QVariant totalSeconds, QVariant percents);

    //! emitted once previously started album art download
    //! request has fully completed.
    void albumArtDownloadProgressed(int percents);

private:

    //! @brief Creates/Initializes a periodic maintenance
    //! task that will scan the directories and report
    //! possible newly added items if any.
    void createMaintainTask();

    //! @brief Tries find a track instance from track db, and also from
    //! playlists that might contain on-demand created tracks from feed items/urls and what so ever.
    TPTrack* findTrack(const QString trackId);

    void applyDefaultSettings();

    void createSearchObjectProvider();

private: // PROTOCOL IMPLEMENTATION

    //! @brief Responds with NAK code to received message.
    //! @param protocol protocol instance where to respond (="the wire")
    //! @param message message to be responded with NAK
    //! @param error optional error description.
    void protocolRespondNAK(TPWebSocketProtocol *protocol, TPWebSocketProtocolMessage message, const QString error = "");
    void protocolRespondACK(TPWebSocketProtocol *protocol, TPWebSocketProtocolMessage message, const QVariantMap headerArgs, const QVariantMap args);

    //! @brief Creates a event that will be delivered to all connected peers
    void protocolReportEvent(const QString eventId, QVariantMap args = QVariantMap());

    //! @brief Generates a event that reports the current volume level
    bool protocolReportVolumeLevel();

private:

    QVariantMap processSearchResults(TPSearchResults *searchResults, TPStoredProcedure *sp);

public slots:

private slots:

    // Triggered by the maintainTimer
    void startMaintainTask();

    // Triggered by maintain thread
    void maintainTaskState(TPFileScanner::State);

    // From FileScanner
    void mediaScannerStateChanged(TPFileScanner::State state);
    void mediaScannerProgress(int currentEntry, int totalEntries);
    void mediaScannerComplete(TPDatabases *db);

    // From PlayerEngine
    void currentTrackChanged(TPTrack *track);
    void currentPlaybackPositionChanged(TPTrack *track, int seconds);
    void playbackOperationsChanged();
    void activePlaylistChanged();

    void albumArtDownloaded(int index);
    void albumArtDownloadComplete();

    //! @brief Generates a event that tells what playback
    //! control operations are possible at the moment
    void protocolReportPlaybackControls();

private:

    // Reports the startup progress in percents.
    void startupProgress(int percents, bool forceEvent = false);

private:

    //! Scanner used to perform maintenance operation
    //! at runtime in order to find new results.
    TPFileScanner* maintainScanner;

    //! Scanner responsible of performing the initialization as whole
    TPFileScanner* scanner;

    //! Player Engine
    TPPlayerProxy *player;

    //! Album art downloader
    TPAlbumArtDownloader *albumArtDownloader;

    TPAlbum* currentlyPlayingAlbum;

    TPAlbum* currentArtDownloadAlbum;

    //! Responsible of automatic album art downloads
    TPAutomaticAlbumArtDownloader *autoArtLoader;

    //! Database container. Owned.
    TPDatabases *db;

    //! Statistics collector, monitors the player
    //! and increases playcounts and things like that.
    TPPlaybackStatisticsCollector *statCollector;

    //! Stored procedure manager, responsible
    //! of managing the stored procedures used
    //! to perform searches through the database(s).
    TPStoredProcedureMgr *spMgr;

    //! Web Socket Protocol used for
    //! communication towards the clients.
    TPWebSocketProtocol *protocol;

    //! Search content provider.
    TPSearchObjectProvider *sob;

    //! Timer used to schedule maintain operations.
    QTimer maintainTimer;

    //! Initialization percentage. <100 when not ready yet to operate.
    int startupProgressPercents;

    //! Volume control for ALSA
    TPALSAVolume *volumeCtrl;
};

#endif // MUSICPLAYERCORE_H
