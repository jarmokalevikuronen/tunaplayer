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

#ifndef TPSEARCHFACADEDATAPROVIDERS_H
#define TPSEARCHFACADEDATAPROVIDERS_H

#include "db/tptrackdb.h"
#include "db/tpartistdb.h"
#include "db/tpalbumdb.h"
#include "db/tpdbtemplate.h"
#include "feed/tpfeedmgr.h"
#include "playlistmgr.h"
#include "playerengine.h"
#include "tpsearchfacadedataproviderinterface.h"
#include "tpschemes.h"

class TPSearchFacadeTrackDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadeTrackDataProvider(TPTrackDB &_db) :
        TPSearchFacadeDataProviderInterface(schemeTrack), db(_db)
    {

    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);

private:

    TPTrackDB &db;
};

class TPSearchFacadeAlbumDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadeAlbumDataProvider(TPAlbumDB &_db) :
        TPSearchFacadeDataProviderInterface(schemeAlbum), db(_db)
    {

    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);

private:

    TPAlbumDB &db;
};

class TPSearchFacadeArtistDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadeArtistDataProvider(TPArtistDB &_db) :
        TPSearchFacadeDataProviderInterface(schemeArtist), db(_db)
    {

    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);

private:

    TPArtistDB &db;
};

class TPSearchFacadeFeedDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadeFeedDataProvider(TPFeedMgr &_db) :
        TPSearchFacadeDataProviderInterface(schemeFeed), db(_db)
    {

    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);

private:

    TPFeedMgr &db;
};


class TPSearchFacadeFeedItemDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadeFeedItemDataProvider(TPFeedMgr &_db) :
        TPSearchFacadeDataProviderInterface(schemeFeedItem), db(_db)
    {
        selectedFeed = 0;
    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);
    void configure(TPSearchFilterOpInterface *rules, TPStoredProcedureArgs *args);

private:

    TPFeedMgr &db;
    TPFeed* selectedFeed;
};

class TPSearchFacadePlaylistDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadePlaylistDataProvider(TPPlaylistMgr &_db) :
        TPSearchFacadeDataProviderInterface(schemePlaylist), db(_db)
    {

    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);

private:

    TPPlaylistMgr &db;
};

class TPSearchFacadePlaylistTrackDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadePlaylistTrackDataProvider(TPPlaylistMgr &_db) :
        TPSearchFacadeDataProviderInterface(schemePlaylistTracks), db(_db)
    {
        selectedPlaylist = 0;
    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);
    void configure(TPSearchFilterOpInterface *rules, TPStoredProcedureArgs *args);

private:

    TPPlaylist *selectedPlaylist;
    TPPlaylistMgr &db;
};

class TPSearchFacadeActivePlaylistDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadeActivePlaylistDataProvider(TPPlayerProxy &_eng) :
        TPSearchFacadeDataProviderInterface(schemeActivePlaylist), eng(_eng)
    {

    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);

private:

    TPPlayerProxy &eng;
};

class TPSearchFacadeCurrentTrackDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadeCurrentTrackDataProvider(TPPlayerProxy &_eng) :
        TPSearchFacadeDataProviderInterface(schemeCurrentTrack), eng(_eng)
    {

    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);

private:

    TPPlayerProxy &eng;
};

class TPSearchFacadeCurrentAlbumDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadeCurrentAlbumDataProvider(TPPlayerProxy &_eng) :
        TPSearchFacadeDataProviderInterface(schemeCurrentAlbum), eng(_eng)
    {

    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);

private:

    TPPlayerProxy &eng;
};

class TPSearchFacadeCurrentArtistDataProvider : public TPSearchFacadeDataProviderInterface
{
public:

    TPSearchFacadeCurrentArtistDataProvider(TPPlayerProxy &_eng) :
        TPSearchFacadeDataProviderInterface(schemeCurrentArtist), eng(_eng)
    {

    }

private: // From TPSearchFacadeDataProviderInterface

    int count();
    TPAssociativeObject* at(int at);

private:

    TPPlayerProxy &eng;
};

#endif // TPSEARCHFACADEDATAPROVIDERS_H
