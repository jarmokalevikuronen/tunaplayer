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

#ifndef TPASSOCIATIVEMETA_H
#define TPASSOCIATIVEMETA_H

#include <QString>
#include <QStringList>

// Separator to differentiate tags from a tag key.
static const QChar tagSeparator(';');

//
// So Called runtime attributes.
//
// Non object type specific meta information keys that are
// not to be distributed away from the software.
//
//
static const QString objectAttrPrimaryKey("_pk_");
static const QString objectAttrIdentifier("id");
static const QString objectAttrScheme("scheme");
static const QString objectAttrTags("tags");
static const QString objectAttrName("name");
static const QString objectAttrCreated("crtt");
static const QString objectAttrUpdated("updt");
static const QString objectAttrLastPlayed("lastplt");
static const QString objectAttrLastPlayedAgo("lastpltago");
static const QString objectAttrLen("len");
static const QString objectAttrPlayCount("plcnt");
static const QString objectAttrPlayLength("pllen");
static const QString objectAttrShortPlays("shortplays");
static const QString objectAttrDeleted("deleted");
static const QString objectAttrAge("age");

// DYNAMIC TOKENS -> NEVER SAVED TO DISK OR ANYTHING
static const QString dynamicTokenPrefix("_d.");
static const QString objectAttrUserTokens_DYNAMIC("_d.usr");
static const QString userTokensDelimiter("|");

//
// TRACK SPECIFIC META INFORMATION KEYS
//
static const QString trackAttrArtistName("album.artist.name");
static const QString trackAttrAlbumName("album.name");
static const QString trackAttrAlbumId("album.id");
static const QString trackAttrArtistId("album.artist.id");
static const QString trackAttrIndex("idx");
static const QString trackAttrFilename("fn");
static const QString trackAttrGenre("genre");
static const QString trackAttrYear("year");
static const QString trackAttrAlbum("album.");
static const QString trackAttrObjectType("type");
   static const QString trackAttrObjectTypeFile("file");
   static const QString trackAttrObjectTypePlaylist("playlist");
   static const QString trackAttrObjectTypeYoutube("youtube");

//
// ALBUM SPECIFIC META INFORMATION KEYS
//
static const QString albumAttrArtSmall("album.arts");
static const QString albumAttrArtLarge("album.artl");
static const QString albumAttrCountTracks("trcnt");
static const QString albumAttrAutomaticAlbumArtDownloadTries("aadltries");
static const QString albumAttrAutomaticAlbumArtLastTry("aadllast");
static const QString albumAttrArtistName("artist.name");
static const QString albumAttrArtistId("artist.id");
static const QString albumAttrArtist("artist.");

//
// ARTIST SPECIFIC META INFORMATION KEYS
//
static const QString artistAttrCountAlbums("alcnt");
static const QString artistAttrCountTracks("trcnt");


//
// Playlist specific meta
//
static const QString playlistAttrTags("tags");
static const QString playlistAttrArtSmall("arts");
static const QString playlistAttrArtLarge("artl");
static const QString playlistAttrFilename("fn");
static const QString playlistAttrCategory("category");
static const QString playlistAttrBuiltIn("builtin");

//
// Feed Item specific meta information
//
static const QString feedItemAttrTitle("title");
static const QString feedItemAttrDescription("desc");
static const QString feedItemAttrLink("link");
static const QString feedItemAttrSize("size");
static const QString feedItemAttrEnclosure("encl");
static const QString feedItemAttrParentUrl("purl");
//
// Feed specific meta information
//
static const QString feedAttrUrl("url");
static const QString feedAttrLink("link");
static const QString feedAttrDescription("desc");
static const QString feedAttrTitle("title");

//
// YOUTUBE specific meta information
//
static const QString youtubeAttrUrl("mediaurl");
static const QString youtubeAttrThumbnailUrl("thumburl");

#endif // TPASSOCIATIVEMETA_H
