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

#ifndef TPSCHEMES_H
#define TPSCHEMES_H

#include <QByteArray>

static const QByteArray schemeTrack("track://");
static const QByteArray schemeAlbum("album://");
static const QByteArray schemeArtist("artist://");
static const QByteArray schemePlaylist("playlist://");
static const QByteArray schemePlaylistTracks("playlisttracks://");
static const QByteArray schemeActivePlaylist("activeplaylist://");
static const QByteArray schemeFeed("feed://");
static const QByteArray schemeFeedItem("feeditem://");
static const QByteArray schemeCurrentTrack("currenttrack://");
static const QByteArray schemeCurrentAlbum("currentalbum://");
static const QByteArray schemeCurrentArtist("currentartist://");
static const QString schemeParameter("parameter://");
static const QString schemeValue("value://");
static const QString schemeObject("object://");
static const QString schemeDlAlbumArt("dlalbumart://");
static const QByteArray schemeYoutubeSearchResults("youtubesearch://");
static const QByteArray schemeYoutube("youtube://");

#endif // TPSCHEMES_H
