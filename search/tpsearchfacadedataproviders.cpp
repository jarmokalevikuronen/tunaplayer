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

#include "tpsearchfacadedataproviders.h"
#include "tpsearchfilterevalargs.h"

int TPSearchFacadeTrackDataProvider::count()
{
    return db.count();
}

TPAssociativeObject* TPSearchFacadeTrackDataProvider::at(int index)
{
    Q_ASSERT(index >= 0 && index < count());
    return db.at(index);
}




int TPSearchFacadeAlbumDataProvider::count()
{
    return db.count();
}

TPAssociativeObject* TPSearchFacadeAlbumDataProvider::at(int index)
{
    Q_ASSERT(index >= 0 && index < count());
    return db.at(index);
}




int TPSearchFacadeArtistDataProvider::count()
{
    return db.count();
}

TPAssociativeObject* TPSearchFacadeArtistDataProvider::at(int index)
{
    Q_ASSERT(index >= 0 && index < count());
    return db.at(index);
}





int TPSearchFacadeFeedDataProvider::count()
{
    return db.count();
}

TPAssociativeObject* TPSearchFacadeFeedDataProvider::at(int index)
{
    Q_ASSERT(index >= 0 && index < count());
    return db.at(index);
}


void TPSearchFacadeFeedItemDataProvider::configure(TPSearchFilterOpInterface *rules, TPStoredProcedureArgs *args)
{
    selectedFeed = 0;
    //
    // Here, we will hopefully pick one of the playlists
    // available based on the rules given.
    //
    for (int i=0;!selectedFeed && i<db.count();++i)
    {
        TPFeed *feed = db.at(i);
        TPSearchFilterEvalArgs a(feed, args);

        if (rules->evaluate(a))
            selectedFeed = feed;
    }
}


int TPSearchFacadeFeedItemDataProvider::count()
{
    return selectedFeed ? selectedFeed->count() : 0;
}

TPAssociativeObject* TPSearchFacadeFeedItemDataProvider::at(int index)
{
    Q_ASSERT(selectedFeed && index >= 0 && index < count());

    return selectedFeed->at(index);
}




int TPSearchFacadePlaylistDataProvider::count()
{
    return db.count();
}


TPAssociativeObject* TPSearchFacadePlaylistDataProvider::at(int index)
{
    Q_ASSERT(index >= 0 && index < count());

    return db.at(index);
}

void TPSearchFacadePlaylistTrackDataProvider::configure(TPSearchFilterOpInterface *rules, TPStoredProcedureArgs *args)
{
    selectedPlaylist = 0;
    //
    // Here, we will hopefully pick one of the playlists
    // available based on the rules given.
    //
    for (int i=0;!selectedPlaylist && i<db.count();++i)
    {
        TPPlaylist *pl = db.at(i);
        TPSearchFilterEvalArgs a(pl, args);

        if (rules->evaluate(a))
            selectedPlaylist = pl;
    }
}

int TPSearchFacadePlaylistTrackDataProvider::count()
{
    return selectedPlaylist ? selectedPlaylist->count() : 0;
}

TPAssociativeObject* TPSearchFacadePlaylistTrackDataProvider::at(int index)
{
    Q_ASSERT(selectedPlaylist);

    return selectedPlaylist->at(index);
}



int TPSearchFacadeActivePlaylistDataProvider::count()
{
    TPPlaylist *pl = eng.getPlaylist();

    return pl ? pl->count() : 0;
}

TPAssociativeObject* TPSearchFacadeActivePlaylistDataProvider::at(int index)
{
    Q_ASSERT(index >= 0 && index < count());

    return eng.getPlaylist()->at(index);
}



int TPSearchFacadeCurrentTrackDataProvider::count()
{
    return eng.getCurrentTrack() ? 1 : 0;
}

TPAssociativeObject* TPSearchFacadeCurrentTrackDataProvider::at(int index)
{
    Q_UNUSED(index);
    Q_ASSERT(index == 0);

    return eng.getCurrentTrack();
}

int TPSearchFacadeCurrentAlbumDataProvider::count()
{
    TPTrack *t = eng.getCurrentTrack();
    return t && t->getAlbum() ? 1 : 0;
}

TPAssociativeObject* TPSearchFacadeCurrentAlbumDataProvider::at(int index)
{
    Q_UNUSED(index);
    Q_ASSERT(eng.getCurrentTrack());

    return eng.getCurrentTrack()->getAlbum();
}

int TPSearchFacadeCurrentArtistDataProvider::count()
{
    TPTrack *t = eng.getCurrentTrack();
    TPAlbum *a = t ? t->getAlbum() : 0;

    return a && a->getArtist() ? 1 : 0;
}

TPAssociativeObject* TPSearchFacadeCurrentArtistDataProvider::at(int index)
{
    Q_UNUSED(index);
    Q_ASSERT(index == 0);

    return eng.getCurrentTrack()->getAlbum()->getArtist();
}



int TPSearchFacadeYouTubeSearchDataProvider::count()
{
    return search.count();
}

TPAssociativeObject* TPSearchFacadeYouTubeSearchDataProvider::at(int index)
{
    return search.at(index);
}

int TPSearchFacadeYouTubeDataProvider::count()
{
    return db.count();
}

TPAssociativeObject* TPSearchFacadeYouTubeDataProvider::at(int index)
{
    return db.at(index);
}

