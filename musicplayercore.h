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
#include "albumartdownloadmodel.h"
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

    bool start();

    //
    // TODO:
    // 1. Need to be able to seek -> use common API available already -> [SEEK]123
    // 2. Playlist manipulation
    //   -> Seeking to specific track within a plays
    //   -> Control the playlist operation mode, repeat
    //   -> To be able to suffle -> must notify also playlist change stuff
    // 4. Täytyy olla oma modeli albumeille -> tälle jo implementaatio osittain olemassa.
    // 5. Täytyy miettiä kuinka ne albumi/artisti shortcutit voisi hoitaa, nythän ne indeksoi suoraan mutta voidaanko sitä käyttää sellaisenaa
    // 6.  Paljon muitakin juttuja, mietitään kun keretään....
    //
    Q_INVOKABLE QVariant currentTrackName() const;
    Q_INVOKABLE QVariant currentArtistName() const;
    Q_INVOKABLE QVariant currentAlbumName() const;
    Q_INVOKABLE QVariant currentTrackLength() const;
    Q_INVOKABLE QImage currentAlbumArt() const;
    QVariant currentTrackId() const;
    QVariant currentAlbumId() const;
    QVariant currentArtistId() const;
    QVariant currentPlaylistId() const;
    // Returns a relative path to art. Relative here means
 //   const QString relativePathToArt(const QString id);

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

    void clearActivePlaylist()
    {
        setActivePlaylist(QString(""));
    }

private slots:

    void onProtocolMessage(TPWebSocketProtocol *protocol, TPWebSocketProtocolMessage message);

public:

    /*
playlist use caset:

1. Tyhjennetään playlista
2. Lisätään joku albumi playlistalle.


-> Pitää vähintään pystyä turaamaan niin, että homma hoituu.

Olisiko kuitenkin niin, että playerillä voisi olla ihan oikeasti
oma playlista, jota siis ei oikeasti ole tallennettu mihinkään. Jos sitten haluaa sen tallentaa, niin sitten siitä
tehdään kopio ja sitä rataa!!!

*/


    bool setActivePlaylist(const QString idOrName);

    //! mode = "queue", "once", "repeat"
    Q_INVOKABLE void setActivePlaylistMode(QVariant mode);
    Q_INVOKABLE QVariant getActivePlaylistMode();

    bool seekToTrack(const QString trackId);


    //! @brief Saves the current (=Player) playlist with given name
    //! but if the name is not given, and the playlist was a cloned one,
    //! the original playlist this playlist was cloned from is to be replaced accordingly.
    bool saveCurrentPlaylist(const QString name);

    bool removePlaylist(const QString id);
#if 0
    Q_INVOKABLE const QString  getActivePlaylistName() const
    {
        Q_ASSERT(playlistModel);
        return playlistModel->playlistName();
    }
#endif


    //! @brief Searches a album art from the supported
    //! services (lastfm, google image search)
    //! @param album or track identifier, E.g. album://a12321312ab.. or track://a1232342..
    bool searchAlbumArt(const QString id)
    {
        Q_ASSERT(albumArtDownloader);

        if (albumArtDownloader->busy())
            return false;

        TPAlbum *album = db->getAlbumDB()->getById(id);
        if (!album)
        {
            TPTrack *track = db->getTrackDB()->getById(id);
            if (track)
                album = track->getAlbum();
        }

        if (!album)
            return false;

        albumArtDownloader->reset();

        TPAlbumArtDownloadRequest *req = new TPAlbumArtDownloadRequest(album, 4, albumArtDownloader);

        bool status = albumArtDownloader->exec(req);
        if (status)
        {
            if (currentArtDownloadAlbum)
                currentArtDownloadAlbum->dec();
            currentArtDownloadAlbum = album;
            currentArtDownloadAlbum->inc();
        }
        else
            // Free the request if startup failed.
            delete req;

        return status;
    }

    void cancelSearchAlbumArt()
    {
        Q_ASSERT(albumArtDownloader);

        if (currentArtDownloadAlbum)
        {
            currentArtDownloadAlbum->dec();
            currentArtDownloadAlbum = NULL;
        }
    }

    bool selectSearchedAlbumArt(const QString id)
    {
        bool success = false;

        if (currentArtDownloadAlbum)
        {
            success = albumArtDownloader->saveImage(id, currentArtDownloadAlbum);
            albumArtDownloader->reset();

            if (success)
            {
                TPTrack *current = player->getCurrentTrack();
                if (current && current->getAlbum() == currentArtDownloadAlbum)
                    // TODO: We should actually emit a protocol event here, should not we?
                    // Lets notify with signal that the album cover that is show somewhere has changed also.
                    emit playingAlbumChanged();
            }

            currentArtDownloadAlbum->dec();
            currentArtDownloadAlbum = NULL;
        }

        return success;
    }

    bool executePlaybackOperation(const QString operation);

#if 0
    inline PlaylistModel* getPlaylistModel() const
    {
        Q_ASSERT(playlistModel);
        return playlistModel;
    }
#endif
    inline TPAlbumArtDownloadModel* getAlbumArtDownloadModel() const
    {
        Q_ASSERT(albumArtDownloader);

        return albumArtDownloader->getModel();
    }

    inline TPPlayerProxy* getPlayer() const
    {
        Q_ASSERT(player);
        return player;        
    }

    void addMediaPath(const QString mediaPath);
    void addMediaPaths(const QStringList _mediaPaths);
    const QStringList getMediaPaths();

    inline TPPlaylistMgr* getPlaylistsModel() const
    {
        return db->getPlaylistDB();
    }

    inline TPFeedMgr* getFeedsModel() const
    {
        return db->getFeedDB();
    }


    //! @brief returns a full path to album art file for a specific object
    //! object can be track, artist, album, playlist or basically whatever
    //! object tjat supports arts in a first place.
    //! @param id identifier to be seached for.
    //! @param type describes the type of image. Accepted values are
    //! "small" => For smaller size of image
    //! "large" => For larger album art image.
    const QString albumArtPath(const QString id, const QString type);

signals:

    void playingTrackChanged();
    void playingAlbumChanged();
    void playbackPositionChanged(QVariant currentSeconds, QVariant totalSeconds, QVariant percents);

    //!
    //! @brief Emitted when possible player operations change.
    //! @param stringOfTokens contains following strings depending whether they are possible
    //! "[PLAY]" -> Playback is possible
    //! "[STOP]" -> Stopping is possible
    //! "[PAUSE]" -> Pausing is possible
    //! "[RESUME]" -> Resuming is possible
    //! "[NEXT]" -> Skip to next song is possible
    void playbackControlStatusUpdate(QVariant stringOfValidTokens);

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
    TPTrack* findTrack(const QString trackId)
    {
        TPTrack *track = db->getTrackDB()->getById(trackId);
        if (track)
            return track;
        // Try to search from playlists that might
        // effectively contain "detached" tracks
        // that are actually not part of the
        // DB:s as such.
        return db->getPlaylistDB()->findTrack(trackId);
    }

    void applyDefaultSettings();

    void createSearchObjectProvider();

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
    // From AlbumArtDownloader
    void albumArtDownloadProgress(int percents, TPAlbumArtDownloadRequest *request, QImage image)
    {
        Q_UNUSED(request);
        Q_UNUSED(image);
        emit albumArtDownloadProgressed(percents < 0 ? 0 : percents >= 100 ? 99 : percents);
    }

    void albumArtDownloadComplete();

    //! @brief Generates a event that tells what playback
    //! control operations are possible at the moment
    void protocolReportPlaybackControls();

private:

    // Reports the startup progress in percents.
    void startupProgress(int percents, bool forceEvent = false);

private:

    QStringList mediaPaths;

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
