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

#include <QTime>

#include "musicplayercore.h"
#include "tpfilescanner.h"
#include "tppathutils.h"
#include "tplog.h"
#include "musicplaybackdefines.h"
#include "tpsearchobjectprovider.h"
#include "tpsearchfacadedataproviders.h"
#include "tpsearchresults.h"
#include "tpsort.h"
#include "operations/tpsearchfilteropfactory.h"
#include "tpsearchfilterevalargs.h"
#include "tpstoredprocedure.h"
#include "tpstoreprocedurerunner.h"
#include "tpprotocoldefines.h"
#include "tpsettings.h"
#include "tpclargs.h"
#include "tpyoutubedb.h"

TPMusicPlayerCore::TPMusicPlayerCore(TPWebSocketProtocol *_protocol) : QObject(NULL)
{
    protocol = _protocol;
    if (protocol)
    {
        connect(protocol, SIGNAL(protocolMessageReceived(TPWebSocketProtocol*,TPWebSocketProtocolMessage)), this, SLOT(onProtocolMessage(TPWebSocketProtocol*,TPWebSocketProtocolMessage)));
        connect(protocol, SIGNAL(protocolClientDisconnected(TPWebSocketProtocol*,int,void*)), this, SLOT(onProtocolClientDisconnected(TPWebSocketProtocol*,int,void*)));
    }

    applyDefaultSettings();

    db = NULL;
    player = NULL;
    scanner = NULL;
    albumArtDownloader = NULL;
    currentArtDownloadAlbum = NULL;
    statCollector = NULL;
    spMgr = NULL;
    sob = NULL;
    maintainScanner = 0;
    startupProgressPercents = 0;
    currentlyPlayingAlbum = 0;
    volumeCtrl = new TPALSAVolume;

    //
    // Youtube search functionality stands here.
    //
    youtubeSearch = new TPYouTubeSearch(this);
    connect(youtubeSearch, SIGNAL(searchComplete()), this, SLOT(youtubeSearchComplete()));

    createMaintainTask();
}

TPMusicPlayerCore::~TPMusicPlayerCore()
{
    if (currentArtDownloadAlbum)
        currentArtDownloadAlbum->dec();
    if (currentlyPlayingAlbum)
        currentlyPlayingAlbum->dec();

    delete volumeCtrl;
    delete spMgr;
    delete statCollector;
    delete player;
    if (scanner)
    {
        scanner->terminate();
        delete scanner;
    }
    if (maintainScanner)
    {
        maintainScanner->terminate();
        delete maintainScanner;
    }
    delete db;
    delete sob;
}

void TPMusicPlayerCore::applyDefaultSettings()
{
    TPSettings &sett = TPSettings::instance();

    // Apply default settings here.
    QString cmdPlayTrack = sett.get(settingPlayTrackCmd, "").toString();
    if (cmdPlayTrack.length() <= 0)
        sett.set(settingPlayTrackCmd, MPLAYER_PLAY_TRACK);

    QString cmdPlayPlaylist = sett.get(settingPlayPlaylistCmd, "").toString();
    if (cmdPlayPlaylist.length() <= 0)
        sett.set(settingPlayPlaylistCmd, MPLAYER_PLAY_PLAYLIST);
}

bool TPMusicPlayerCore::start()
{
    if (scanner)
        return false;

    // Create scanner and connect to it. Then start the parsing.
    scanner = new TPFileScanner(db, TPPathUtils::getMediaPaths(), TPPathUtils::getMusicDbFilename());
    QObject::connect(scanner, SIGNAL(mediaScannerProgress(int,int)), this, SLOT(mediaScannerProgress(int,int)));
    QObject::connect(scanner, SIGNAL(mediaScannerStateChanged(TPFileScanner::State)), this, SLOT(mediaScannerStateChanged(TPFileScanner::State)));
    QObject::connect(scanner, SIGNAL(mediaScannerComplete(TPDatabases*)), this, SLOT(mediaScannerComplete(TPDatabases*)));

    scanner->start();

    // Album art download functionality can be started immediately
    albumArtDownloader = new TPAlbumArtDownloader(this);
    connect(albumArtDownloader, SIGNAL(downloadComplete()), this, SLOT(albumArtDownloadComplete()));
    connect(albumArtDownloader, SIGNAL(albumArtDownloaded(int)), this, SLOT(albumArtDownloaded(int)));

    // Instantiate stored procedure manager that is responsible
    // of maintaining stored procedures if any.
    spMgr = new TPStoredProcedureMgr();

    return true;
}

//
// SLOTS from PlayerEngine
//

bool TPMusicPlayerCore::executePlaybackOperation(const QString op)
{
    return player ? player->Execute(op) : false;
}

void TPMusicPlayerCore::playbackOperationsChanged()
{
    protocolReportPlaybackControls();
}

void TPMusicPlayerCore::activePlaylistChanged()
{
    protocolReportPlaybackControls();
}


//
// SLOTS from the filescanner
//

void TPMusicPlayerCore::mediaScannerStateChanged(TPFileScanner::State state)
{
    switch (state)
    {
        case TPFileScanner::MediaScannerUnknown:
        case TPFileScanner::MediaScannerMaintainScanComplete:
        break;

        case TPFileScanner::MediaScannerFullScanDisk:
            startupProgress(1);
            break;
        case TPFileScanner::MediaScannerFullProcessFiles:
            break;
        case TPFileScanner::MediaScannerFullExportDB:
            break;
        case TPFileScanner::MediaScannerUpgradeImportDB:
            startupProgress(10);
            break;
        case TPFileScanner::MediaScannerUpgradeExportDB:
            break;
        case TPFileScanner::MediaScannerUpgradeCheckNew:
            startupProgress(55);
            break;
        case TPFileScanner::MediaScannerUpgradeVerifyExisting:
            startupProgress(25);
            break;
        case TPFileScanner::MediaScannerUpgradeScanDisk:
            startupProgress(50);
            break;
        case TPFileScanner::MediaScannerComplete:
            startupProgress(99);
            break;
    }
}

void TPMusicPlayerCore::mediaScannerComplete(TPDatabases *_db)
{
    if (!db)
    {
        db = _db;

        db->getFeedDB();
        db->getPlaylistDB();
    }

    Q_ASSERT(db == _db);

    DEBUG() << "CORE: STATISTICS: TRACKS:  " << db->getTrackDB()->count();
    DEBUG() << "CORE: STATISTICS: ALBUMS:  " << db->getAlbumDB()->count();
    DEBUG() << "CORE: STATISTICS: ARTISTS: " << db->getArtistDB()->count();

    // Create player engine
    // TODO: Rename player to Player_MPlayerImpl and do define a abstract interface to
    // Access the player more conviniently.
    PlayerBackend_MPlayer *playerImpl = new PlayerBackend_MPlayer(this);
    player = new TPPlayerProxy(playerImpl, this);

    connect(player, SIGNAL(playbackOperationsChanged()), this, SLOT(playbackOperationsChanged()));
    connect(player, SIGNAL(currentTrackChanged(TPTrack*)), this, SLOT(currentTrackChanged(TPTrack*)));
    connect(player, SIGNAL(currentPlaybackPositionChanged(TPTrack*,int)), this, SLOT(currentPlaybackPositionChanged(TPTrack*,int)));
    connect(player, SIGNAL(activePlaylistChanged()), this, SLOT(activePlaylistChanged()));

    //
    // Create statistics collector
    // that will update tracks/albums based on the
    // playback information
    //
    statCollector = new TPPlaybackStatisticsCollector(this);
    connect(player, SIGNAL(currentTrackChanged(TPTrack*)), statCollector, SLOT(currentTrackChanged(TPTrack*)));
    connect(player, SIGNAL(currentPlaybackPositionChanged(TPTrack*,int)), statCollector, SLOT(playbackPositionChanged(TPTrack*,int)));

    autoArtLoader = new TPAutomaticAlbumArtDownloader(this);
    connect(autoArtLoader, SIGNAL(albumArtDownloadComplete()), this, SLOT(albumArtDownloadComplete()));

    // Do execute automatic album art loader
    // if not explicitly instructed not to.

    bool autoartDisabled = TPSettings::instance().get(settingDisableAutoArtLoader, false).toBool();
    if (!autoartDisabled)
        autoArtLoader->execute(db->getAlbumDB());

    if (scanner)
    {
        scanner->deleteLater();
        scanner = NULL;
    }

    createSearchObjectProvider();

    // Inform callers that we are more or less ready here.
    startupProgress(100);
}

void TPMusicPlayerCore::mediaScannerProgress(int currentEntry, int totalEntries)
{
    int percents = totalEntries ? ((currentEntry * 100) / totalEntries) : 99;
    if (percents > 99)
        percents = 99;

    startupProgress(percents);
}

void TPMusicPlayerCore::createSearchObjectProvider()
{
    if (sob)
        return;

    sob = new TPSearchObjectProvider;
    Q_ASSERT(sob);

    sob->addProvider(new TPSearchFacadeTrackDataProvider(*db->getTrackDB()));
    sob->addProvider(new TPSearchFacadeAlbumDataProvider(*db->getAlbumDB()));
    sob->addProvider(new TPSearchFacadeArtistDataProvider(*db->getArtistDB()));
    sob->addProvider(new TPSearchFacadePlaylistDataProvider(*db->getPlaylistDB()));
    sob->addProvider(new TPSearchFacadePlaylistTrackDataProvider(*db->getPlaylistDB()));
    sob->addProvider(new TPSearchFacadeFeedDataProvider(*db->getFeedDB()));
    sob->addProvider(new TPSearchFacadeFeedItemDataProvider(*db->getFeedDB()));
    sob->addProvider(new TPSearchFacadeActivePlaylistDataProvider(*this->player));
    sob->addProvider(new TPSearchFacadeCurrentTrackDataProvider(*this->player));
    sob->addProvider(new TPSearchFacadeCurrentAlbumDataProvider(*this->player));
    sob->addProvider(new TPSearchFacadeCurrentArtistDataProvider(*this->player));
    sob->addProvider(new TPSearchFacadeYouTubeSearchDataProvider(*this->youtubeSearch));
    sob->addProvider(new TPSearchFacadeYouTubeDataProvider(*db->getYouTubeDB()));
}

bool TPMusicPlayerCore::addToPlaylist(QVariant aObject, const QString &position)
{
    Q_ASSERT(player);

    bool toBack = true;

    if (position == protocolCommandAddToPlaylistArgPositionValueFront)
        toBack = false;
    else if (position == protocolCommandAddToPlaylistArgPositionValueBack)
        toBack = true;

    QString id = aObject.toString();

    TPPlaylist *pl = player->getPlaylist();
    if (!pl)
        pl = db->getPlaylistDB()->createEmptyPlaylist();

    TPTrack *track = db->getTrackDB()->getById(id);
    if (track)
        pl->add(track, toBack);
    else
    {
        TPAlbum *album = db->getAlbumDB()->getById(id);
        if (album)
            pl->add(album, toBack);
        else
        {
            TPArtist *artist = db->getArtistDB()->getById(id);
            if (artist)
                pl->add(artist, toBack);
            else
            {
                TPPlaylist *pladd = db->getPlaylistDB()->getPlaylistById(id);
                if (pladd)
                    pl->add(pladd, toBack);
                else
                {
                    TPFeed *feed = db->getFeedDB()->getById(id);
                    if (feed)
                        pl->add(feed, toBack);
                    else
                    {
                        TPFeedItem *feedItem = db->getFeedDB()->getFeedItemById(id);
                        if (feedItem)
                            pl->add(feedItem, toBack);
                        else
                        {
                            TPYouTubeObject *uo = youtubeSearch->findById(id);
                            if (!uo)
                                uo = db->getYouTubeDB()->getById(id);
                            if (uo)
                                pl->add(uo, toBack);
                            else
                                return false;
                        }
                    }
                }
            }
        }
    }

    if (player->getPlaylist() != pl)
    {
        // NOTE: Player takes the ownership of the new playlist.
        player->setPlaylist(pl);
        pl->dec();
    }

    // This will emit signal about the potentially changed pbk operations.
    playbackOperationsChanged();

    return true;
}

bool TPMusicPlayerCore::removePlaylist(const QString id)
{
    return db->getPlaylistDB()->removePlaylist(id);
}

bool TPMusicPlayerCore::removeFromPlaylist(const QString id)
{
    if (id.startsWith(schemeArtist))
    {
        TPArtist *artist = db->getArtistDB()->getById(id);
        if (!artist)
            return false;
        DEBUG() << "CORE: removeArtistFromPlaylist: " << id;
        for (int i=0;i<artist->countAlbums();i++)
            removeFromPlaylist(artist->getAlbum(i)->identifier(true));

        return true;
    }
    else if (id.startsWith(schemeAlbum))
    {
        TPAlbum *album = db->getAlbumDB()->getById(id);
        if (!album)
            return false;
        DEBUG() << "CORE: removeAlbumFromPlaylist: " << id;
        for (int i=0;i<album->countTracks();i++)
            removeFromPlaylist(album->getTrackByPos(i)->identifier(true));

        return true;
    }
    else
    {
        TPTrack *track = findTrack(id);
        if (track)
        {
            DEBUG() << "CORE: removeTrackFromPlaylist: " << id;
            TPPlaylist *pl = player->getPlaylist();
            if (pl)
            {
                TPPlaylist *clonedFrom = pl->getClonedFrom();

                pl->remove(track);

                TPPlaylistUtils::exportLocalPlaylist(pl, clonedFrom ? clonedFrom->getFilename() : pl->getFilename());
                return true;
            }
        }
    }

    return false;
}

Q_INVOKABLE void TPMusicPlayerCore::setActivePlaylistMode(QVariant mode)
{
    Q_ASSERT(player);

    QString modeStr(mode.toString());

    player->setMode(modeStr);
}

Q_INVOKABLE QVariant TPMusicPlayerCore::getActivePlaylistMode()
{
    Q_ASSERT(player);

    if (!player->getPlaylist())
        return QVariant("");

    return QVariant(player->getMode());
}

bool TPMusicPlayerCore::seekToTrack(const QString trackId)
{
    Q_ASSERT(player);

    TPPlaylist *pl = player->getPlaylist();
    if (!pl)
        return false;

    TPTrack *track = db->getTrackDB()->getById(trackId);
    if (!track)
        return false;

    if (!pl->hasTrack(track))
        return false;

    return player->seekToTrack(track);
}

bool TPMusicPlayerCore::setActivePlaylist(const QString nameOrId, const QString userprofile)
{
    DEBUG() << "CORE: setActivePlaylist(" << nameOrId << "," << userprofile << ")";

    TPPlaylist *playlist = NULL;

    if (nameOrId.length())
        playlist = db->getPlaylistDB()->getPlaylist(nameOrId);

    // Take copy of the playlist -> player always has its own instance
    // so that it can modify it as it wishes.
    if (playlist)
    {
        if (playlist->needToBeCloned())
        {
            TPPlaylist *tmp = db->getPlaylistDB()->createEmptyPlaylist();
            if (tmp)
            {
                tmp->cloneFrom(playlist);
                playlist = tmp;
            }
        }
    }

    //
    // If random playlist -> set user profile if provided.
    //
    if (playlist && nameOrId == randomPlaylistName && userprofile.length())
    {
        playlist->setUserProfile(userprofile);
    }

    // NOTE: Player takes ownership of this playlist instance that
    // was previously cloned. (if any, null is valid value also).
    player->setPlaylist(playlist);

    playbackOperationsChanged();

    // notify all clients that current playlist has changed..
    protocolReportEvent(protocolEventCurrentPlaylistChanged);

    return true;
}

//! @brief Saves the current (=Player) playlist with given name
//! but if the name is not given, and the playlist was a cloned one,
//! the original playlist this playlist was cloned from is to be replaced accordingly.
bool TPMusicPlayerCore::saveCurrentPlaylist(const QString name)
{
    Q_ASSERT(player);

    TPPlaylist *pl = player->getPlaylist();
    if (!pl || !pl->count())
        return false;

    if (name.length() <= 0 && pl->getClonedFrom())
    {
        // This playlist was a cloned one -> Do save the original one.
        bool success =  TPPlaylistUtils::exportLocalPlaylist(pl, pl->getClonedFrom()->getFilename());
        if (success)
            return db->getPlaylistDB()->reloadPlaylist(db, pl->getClonedFrom());
        return false;
    }

    QString uniqueName = db->getPlaylistDB()->uniquePlaylistName(name);

    TPPlaylist *newPlaylist = db->getPlaylistDB()->createPlaylist(uniqueName);

    // Add the exactly same content to new playlist.
    newPlaylist->add(pl);

    if (TPPlaylistUtils::exportLocalPlaylist(newPlaylist))
    {
        db->getPlaylistDB()->addPlaylist(newPlaylist);
        return true;
    }

    // will be destroyed right here since failed to write a file.
    newPlaylist->dec();

    return false;
}


void TPMusicPlayerCore::protocolReportEvent(const QString eventId, QVariantMap args)
{
    TPWebSocketProtocolMessage event(&evtFilters);
    event.initializeEvent(eventId);
    event.setArgs(args);
    protocol->sendEvent(event);
}

void TPMusicPlayerCore::albumArtDownloaded(int index)
{
    QVariantMap args;

    args.insert(protocolEventAlbumArtSearchItemArgIdKey, QString("dlalbumart://") + QString::number(index));
    args.insert(protocolEventAlbumArtSearchItemArgArtKey, TPPathUtils::getNormalizedPathForWebServer(albumArtDownloader->getFullPathToFile(index)));

    protocolReportEvent(protocolEventAlbumArtSearchItem, args);
}

void TPMusicPlayerCore::albumArtDownloadComplete()
{
    DEBUG() << "CORE: STATE: albumArtDownloadComplete";

    protocolReportEvent(protocolEventAlbumArtSearchComplete);
}

void TPMusicPlayerCore::currentTrackChanged(TPTrack *track)
{
    emit playingTrackChanged();
    protocolReportEvent(protocolEventPlaybackTrackChanged);

    TPAlbum *newAlbum = track ? track->getAlbum() : 0;

    if (currentlyPlayingAlbum != newAlbum)
    {
        if (currentlyPlayingAlbum)
            currentlyPlayingAlbum->dec();

        currentlyPlayingAlbum = newAlbum;
        if (currentlyPlayingAlbum)
            currentlyPlayingAlbum->inc();

        emit playingAlbumChanged();

        protocolReportEvent(protocolEventPlaybackAlbumChanged);
    }

    protocolReportEvent(protocolEventCurrentPlaylistChanged);
}

void TPMusicPlayerCore::currentPlaybackPositionChanged(TPTrack *track, int seconds)
{
    QVariantMap args;
    int percents = -1;

    if (track)
    {
        int len = track->getLen();

        if (len <= 0)
            emit playbackPositionChanged(seconds, 0, 0);
        else
        {
            if (len >= seconds)
                percents = (seconds * 100) / len;

            emit playbackPositionChanged(seconds, len, percents);
        }
    }
    else
    {
        percents = -1;
        seconds = -1;
    }

    args.insert(protocolEventPlaybackPositionChangedArgPositionSecondsKey, seconds);
    args.insert(protocolEventPlaybackPositionChangedArgPositionPercentsKey, percents);

    protocolReportEvent(protocolEventPlaybackPositionChanged, args);
}

bool TPMusicPlayerCore::protocolReportVolumeLevel()
{
    int percents = 0;
    bool ok = volumeCtrl->getVolume(&percents);

    if (ok)
    {
        QVariantMap args;
        args.insert(protocolEventVolumeChangedArgPercentsKey, percents);

        TPWebSocketProtocolMessage event(&evtFilters);
        event.initializeEvent(protocolEventVolumeChanged);
        event.setArgs(args);
        protocol->sendEvent(event);
    }

    return ok;
}

void TPMusicPlayerCore::protocolReportPlaybackControls()
{
    QVariantMap args;
    QVariantMap argItems;

    argItems.insert(playerCmdPlay,   player ? player->CanExecute(playerCmdPlay)   : false);
    argItems.insert(playerCmdStop,   player ? player->CanExecute(playerCmdStop)   : false);
    argItems.insert(playerCmdNext,   player ? player->CanExecute(playerCmdNext)   : false);
    argItems.insert(playerCmdPause,  player ? player->CanExecute(playerCmdPause)  : false);
    argItems.insert(playerCmdMute,   player ? player->CanExecute(playerCmdMute)   : false);
    argItems.insert(playerCmdUnmute, player ? player->CanExecute(playerCmdUnmute) : false);
    argItems.insert(playerCmdSeek,   player ? player->CanExecute(playerCmdSeek)   : false);

    args.insert(protocolEventPlaybackControlsChangedArgControlsKey, argItems);

    TPWebSocketProtocolMessage event(&evtFilters);
    event.initializeEvent(protocolEventPlaybackControlsChanged);
    event.setArgs(args);
    protocol->sendEvent(event);
}

void TPMusicPlayerCore::protocolRespondNAK(TPWebSocketProtocol *protocol, TPWebSocketProtocolMessage message, const QString error)
{
    TPWebSocketProtocolMessage response;

    response.initializeResponseTo(message);
    response.setResponseStatus(protocolExecStatusError, error);

    protocol->sendResponse(response);
}

void TPMusicPlayerCore::protocolRespondACK(TPWebSocketProtocol *protocol,
                                           TPWebSocketProtocolMessage message,
                                           const QVariantMap headerArgs,
                                           const QVariantMap args)
{
    TPWebSocketProtocolMessage response;

    response.initializeResponseTo(message);
    response.setResponseStatus(protocolExecStatusOK);
    response.setArgs(args);

    QVariantMap::const_iterator it = headerArgs.constBegin();
    while (it != headerArgs.constEnd())
    {
        response.setHeaderValue(it.key(), it.value());
        ++it;
    }

    protocol->sendResponse(response);
}

QVariantMap TPMusicPlayerCore::processSearchResults(TPSearchResults *searchResults, TPStoredProcedure *sp)
{
    Q_ASSERT(searchResults);

    QVariantMap result;
    QVariantList items;

    QString groupBy = sp->getGroupBy();

    // Firstly, just a simple sanity check so
    // that grouping can be applied in a first place..
    if (groupBy.length())
    {
        bool fail = false;
        for (int i=0;!fail && i<searchResults->count();++i)
            fail = searchResults->at(i).getString(groupBy).length() <= 0;

        if (fail)
            groupBy.clear();
    }


    if (groupBy.length())
    {
        // Apply grouping.
        QVariantList groupCollector;
        QString currentGroup;

        for (int i=0;i<searchResults->count();++i)
        {
            TPAssociativeObject &ao(const_cast<TPAssociativeObject &>(searchResults->at(i)));
            QString aoGroup = ao.getString(groupBy);

            if (aoGroup != currentGroup)
            {
                if (groupCollector.count() > 0)
                {
                    QVariantMap itemMap;
                    itemMap[protocolCommandExecSPResponseArgItemKey] = groupCollector;
                    items.append(itemMap);
                    groupCollector.clear();
                }
                currentGroup = aoGroup;
            }
            groupCollector.append(ao.toMap(sp->getFilterKeys()));
        }

        if (groupCollector.count() > 0)
        {
            QVariantMap itemMap;
            itemMap[protocolCommandExecSPResponseArgItemKey] = groupCollector;
            items.append(itemMap);
        }
    }
    else
    {
        for (int i=0;i<searchResults->count();++i)
        {
            TPAssociativeObject &ao(const_cast<TPAssociativeObject &>(searchResults->at(i)));
            items.append(ao.toMap(sp->getFilterKeys()));
        }
    }

    result[protocolCommandExecSPResponseArgItemsKey] = items;

    return result;
}

void TPMusicPlayerCore::onProtocolClientDisconnected(TPWebSocketProtocol *protocol, int clientCount, void *clientHandle)
{
    Q_UNUSED(protocol);
    Q_UNUSED(clientCount);

    evtFilters.removeClient(clientHandle);
}

void TPMusicPlayerCore::onProtocolMessage(TPWebSocketProtocol *protocol, TPWebSocketProtocolMessage message)
{
    QTime t; t.start();

    if (message.isCommand())
    {
        QString msgId = message.id();

        if (msgId == protocolCommandAddToPlaylist)
        {
            QVariantMap args = message.arguments();
            QVariant objectId = args[protocolCommandAddToPlaylistArgIdKey];
            QString position = args[protocolCommandAddToPlaylistArgPositionKey].toString();

            if (objectId.isValid() && !objectId.isNull())
            {
                if (addToPlaylist(objectId, position))
                {
                    protocolRespondACK(protocol, message);
                    protocolReportEvent(protocolEventCurrentPlaylistChanged);
                }
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandClearPlaylist)
        {
            clearActivePlaylist();
            protocolRespondACK(protocol, message);
        }
        else if (msgId == protocolCommandSavePlaylist)
        {
            QVariantMap args = message.arguments();
            QVariant name = args[protocolCommandSavePlaylistArgNameKey];
            if (name.isValid() && !name.isNull())
            {
                if (saveCurrentPlaylist(name.toString()))
                    protocolRespondACK(protocol, message);
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandRemovePlaylist)
        {
            QVariantMap args = message.arguments();
            QVariant id = args[protocolCommandRemovePlaylistArgIdKey];
            if (id.isValid() && !id.isNull())
            {
                if (removePlaylist(id.toString()))
                    protocolRespondACK(protocol, message);
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandRemoveFromPlaylist)
        {
            QVariantMap args = message.arguments();
            QVariant id = args[protocolCommandRemoveFromPlaylistArgIdKey];
            if (id.isValid() && !id.isNull())
            {
                if (removeFromPlaylist(id.toString()))
                {
                    protocolRespondACK(protocol, message);
                    // Notify all connected clients that current playlist has changed.
                    protocolReportEvent(protocolEventCurrentPlaylistChanged);
                }
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandSetActivePlaylist)
        {
            QVariantMap args = message.arguments();
            QVariant id = args[protocolCommandSetActivePlaylistArgIdKey];
            QString userprofile = args[protocolArgumentUserProfile].toString();
            if (id.isValid() && !id.isNull())
            {
                if (setActivePlaylist(id.toString(), userprofile))
                    protocolRespondACK(protocol, message);
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandShufflePlaylist)
        {
            TPPlaylist *pl = player->getPlaylist();
            if (pl)
                pl->shuffle();
            protocolRespondACK(protocol, message);
            if (pl)
                protocolReportEvent(protocolEventCurrentPlaylistChanged);
        }
        else if (msgId == protocolCommandSeekToTrack)
        {
            QVariant id = message.arguments()[protocolCommandSeekToTrackArgIdKey];
            if (id.isValid() && !id.isNull())
            {
                if (seekToTrack(id.toString()))
                    protocolRespondACK(protocol, message);
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandSearchAlbumArt)
        {
            QVariantMap args = message.arguments();
            QVariant objectId = args[protocolCommandSearchAlbumArtArgIdKey];
            if (objectId.isValid() && !objectId.isNull())
            {
                if (searchAlbumArt(objectId.toString()))
                    protocolRespondACK(protocol, message);
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandSearchAlbumArtCancel)
        {
            cancelSearchAlbumArt();
            protocolRespondACK(protocol, message);
        }
        else if (msgId == protocolCommandSearchAlbumArtSelect)
        {
            QVariantMap args = message.arguments();
            QVariant objectId = args[protocolCommandSearchAlbumArtSelectArgIdKey];
            if (objectId.isValid() && !objectId.isNull())
            {
                if (selectSearchedAlbumArt(objectId.toString()))
                {
                    protocolRespondACK(protocol, message);
                }
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandPlayerControl)
        {
            QVariantMap args = message.arguments();
            QVariant opCode = args[protocolCommandPlayerControlArgCodeKey];
            if (opCode.isValid() && !opCode.isNull())
            {
                if (executePlaybackOperation(opCode.toString()))
                    protocolRespondACK(protocol, message);
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);

        }
        else if (msgId == protocolCommandStatus)
        {
            // Firstly ACK
            protocolRespondACK(protocol, message);

            // Secondly deliver the state events.
            startupProgress(startupProgressPercents, true);
            protocolReportPlaybackControls();
            protocolReportVolumeLevel();
            if (player)
                currentTrackChanged(player->getCurrentTrack());
        }
        else if (msgId == protocolCommandSetVolume)
        {
            int percents = message.arguments()[protocolCommandSetVolumeArgPercentsKey].toInt();

            bool ok = volumeCtrl->setVolume(percents);
            if (ok)
            {
                if (protocolReportVolumeLevel())
                    protocolRespondACK(protocol, message);
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandReportVolume)
        {
            if (protocolReportVolumeLevel())
                protocolRespondACK(protocol, message);
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandAddEventFilter)
        {
            QVariantMap args = message.arguments();
            QString eventName = args[protocolCommandAddRemoveEventFilterArgIdEventId].toString();
            if (eventName.length())
            {
                evtFilters.addFilteredEvent(message.getOrigin(), eventName);
                protocolRespondACK(protocol, message);
            }
            else
            {
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
        }
        else if (msgId == protocolCommandRemoveEventFilter)
        {
            QVariantMap args = message.arguments();
            QString eventName = args[protocolCommandAddRemoveEventFilterArgIdEventId].toString();
            if (eventName.length())
            {
                evtFilters.removeFilteredEvent(message.getOrigin(), eventName);
                protocolRespondACK(protocol, message);
            }
            else
            {
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
        }
        else if (msgId == protocolCommandGetUserProfiles)
        {
            if (!db || !db->getUserDB())
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionNotReady);

            QStringList profiles = db->getUserDB()->allTags();
            QVariantMap args;

            args.insert(protocolCommandGetUserProfilesArgProfilelist, profiles);
            protocolRespondACK(protocol, message, QVariantMap(), args);
        }
        else if (msgId == protocolCommandYoutubeSearch)
        {
            QString criteria = message.arguments().value(protocolCommandYoutubeSearchArgCriteria).toString();

            if (criteria.length())
            {
                if (youtubeSearch->search(criteria))
                    protocolRespondACK(protocol, message);
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionNotReady);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }
        else if (msgId == protocolCommandYoutubeSave)
        {
            QString id = message.arguments().value(protocolCommandYoutubeSaveArgId).toString();
            if (id.length())
            {
                TPYouTubeObject *object = youtubeSearch->takeById(id);
                if (object)
                {
                    db->getYouTubeDB()->insertItem(object);
                    protocolRespondACK(protocol, message);

                    protocolReportEvent(protocolEventYoutubeDbChanged);
                }
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
            }
            else
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionArgument);
        }

        else if (msgId == protocolCommandExecSP)
        {
            if (!sob)
            {
                protocolRespondNAK(protocol, message, protocolExecErrorDescriptionNotReady);
                return;
            }

            QVariantMap args = message.arguments();
            QString spName =
                    args[protocolCommandExecSPArgNameKey].toString();
            QVariantMap spArgs =
                    args[protocolCommandExecSPArgArgumentsKey].toMap();

            if (spName.length())
            {
                TPStoredProcedure *sp = spMgr->get(spName);
                if (sp)
                {
                    TPSearchResults *results =
                            TPStoredProcedureRunner::execute(spArgs, *sp, *sob);
                    if (results)
                    {
                        // Some header args here..
                        QVariantMap headerArgs;
                        headerArgs[protocolCommandExecSPResponseArgNameKey] = spName;
                        headerArgs[protocolCommandExecSPResponseArgArgsKey] = spArgs;
                        QVariantMap args = processSearchResults(results, sp);

                        protocolRespondACK(protocol, message, headerArgs, args);
                        delete results;
                    }
                    else
                        protocolRespondNAK(protocol, message, protocolExecErrorDescriptionInternalError);
                }
                else
                    protocolRespondNAK(protocol, message, protocolExecErrorDescriptionUnknownSP);
            }
        }
    }
    else
        ERROR() << "CORE: received a message that is not command?";

    PERF() << "CORE: MSG-PROCESSING-TIME: " << t.elapsed() << "ms";
}

void TPMusicPlayerCore::createMaintainTask()
{
    connect(&maintainTimer, SIGNAL(timeout()), this, SLOT(startMaintainTask()));
    maintainTimer.setSingleShot(false);
    int maintainInterval = TPCLArgs::instance().arg(TPCLArgs::cliArgMaintainInterval, 30).toInt();

    // Range the maintain interval to 1min...24hrs..
    maintainInterval = qMax(maintainInterval, 1);
    maintainInterval = qMin(maintainInterval, 60 * 24);

    // Start timer, do minute -> millisecond conversion.
    maintainTimer.start(maintainInterval * 60 * 1000);
}

void TPMusicPlayerCore::maintainTaskState(TPFileScanner::State state)
{
    if (state != TPFileScanner::MediaScannerMaintainScanComplete)
        return;

    QVector<TPAssociativeDBItem *> *results =
            maintainScanner->maintainResults();

    QVector<TPUserTag *> *tags =
            maintainScanner->maintainUserTags();

    if (tags)
    {
        if (db->getUserDB()->upgrade(tags))
        {
            DEBUG() << "CORE: Updating track database with new access tags.";

            // OK. We need to apply here new tags for the whole
            // track DB... not so nice in the main thread but will still be done here.
            TPTrackDB *tdb = db->getTrackDB();
            TPUserManager *udb = db->getUserDB();

            for (int i=0;i<tdb->count();++i)
            {
                TPTrack *t = tdb->at(i);
                QString userProfile = udb->tagStringForFile(t->getFilename());
                //DEBUG() << "CORE: File=" << t->getFilename() << " userProfile=" << userProfile;
                t->setString(objectAttrUserTokens_DYNAMIC, userProfile);
            }
        }
    }

    if (results)
    {
        bool dbChanged = results->count() > 0;

        STATE() << "CORE: Maintain complete: " << results->count();

        if (results->count())
        {
            for (int i=0;i<results->count();++i)
                db->getTrackDB()->insertItem(results->at(i));

            db->getTrackDB()->executePostCreateTasks();
        }

        results->clear();

        if (dbChanged)
            protocolReportEvent(protocolEventDatabaseChanged);

        bool autoartDisabled = TPSettings::instance().get(settingDisableAutoArtLoader, false).toBool();
        if (!autoartDisabled && autoArtLoader)
        {
            DEBUG() << "Triggering automatic album art download";
            autoArtLoader->execute(db->getAlbumDB());
        }

        delete results;
    }
}

void TPMusicPlayerCore::startMaintainTask()
{
    if (autoArtLoader)
    {
        bool autoartDisabled = TPSettings::instance().get(settingDisableAutoArtLoader, false).toBool();
        if (autoartDisabled)
        {
            DEBUG() << "CORE: Automatic album download started...\n";
            autoArtLoader->execute(db->getAlbumDB());
        }
    }

    if (maintainScanner)
    {
        if (maintainScanner->getState() == TPFileScanner::MediaScannerMaintainScanComplete)
        {
            maintainScanner->deleteLater();
            maintainScanner = 0;
        }
    }

    if (!maintainScanner && db)
    {
        maintainScanner = new TPFileScanner(TPPathUtils::getMediaPaths(), db->getTrackDB()->getTrackFilenames(), db->getTrackDB()->associativeDb());
        connect(maintainScanner, SIGNAL(mediaScannerStateChanged(TPFileScanner::State)), this, SLOT(maintainTaskState(TPFileScanner::State)));
        maintainScanner->start();
    }
    else
    {
        STATE() << "CORE: maintain Task skipped - previous still running";
    }
}

void TPMusicPlayerCore::startupProgress(int percents, bool force)
{
    if (percents > startupProgressPercents || force)
    {
        STATE() << "CORE: reportingStartupProgress: " << percents;
        startupProgressPercents = percents;
        QVariantMap args;
        args.insert(protocolEventStartupProgressArgPercents, startupProgressPercents);

        protocolReportEvent(protocolEventStartupProgress, args);
    }
}

bool TPMusicPlayerCore::searchAlbumArt(const QString id)
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

void TPMusicPlayerCore::cancelSearchAlbumArt()
{
    Q_ASSERT(albumArtDownloader);

    if (currentArtDownloadAlbum)
    {
        currentArtDownloadAlbum->dec();
        currentArtDownloadAlbum = NULL;
    }
}

bool TPMusicPlayerCore::selectSearchedAlbumArt(const QString id)
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
            {
                // Lets notify with signal that the album cover that is show somewhere has changed also.
                protocolReportEvent(protocolEventPlaybackAlbumChanged);
                emit playingAlbumChanged();
            }

            protocolReportEvent(protocolEventAlbumChanged, currentArtDownloadAlbum->toMap());
        }

        currentArtDownloadAlbum->dec();
        currentArtDownloadAlbum = NULL;
    }

    return success;
}

void TPMusicPlayerCore::clearActivePlaylist()
{
    setActivePlaylist(QString(""), QString(""));
}


TPTrack* TPMusicPlayerCore::findTrack(const QString trackId)
{
    TPTrack *track = db->getTrackDB()->getById(trackId);
    if (track)
        return track;

    // Try to search from playlists that might
    // effectively contain "detached" tracks
    // that are actually not part of the
    // DB:s as such.
    track = db->getPlaylistDB()->findTrack(trackId);
    if (track)
        return track;

    if (player->getPlaylist())
        return player->getPlaylist()->findTrack(trackId);

    return 0;
}

void TPMusicPlayerCore::youtubeSearchComplete()
{
    protocolReportEvent(protocolEventYoutubeSearchFinished);
}

