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

#include <QFile>
#include <QDebug>
#include "playlistutils.h"
#include "tpassociative.h"
#include "tpassociativemeta.h"


static const QString M3UHEADER("#EXTM3U");
static const QString M3UNAMETAG("#name ");
static const QString M3USTREAMEDTAG("#streamed ");
static const QString M3UARTSMALLTAG("#artsmall ");
static const QString M3UARTBIGTAG("#artbig ");
static const QString M3UCATEGORYTAG("#category ");

bool TPPlaylistUtils::removeLocalPlaylist(TPPlaylist *playlist)
{
    qDebug() << "PLAYLIST: RemovingFile: " << playlist->getFilename();
    QFile::remove(playlist->getFilename());
    return true;
}

TPPlaylist* TPPlaylistUtils::importLocalPlaylist(TPTrackDB *trackDB, TPAlbumDB *albumDB, TPAssociativeDB *db, QString m3ufile)
{
    Q_UNUSED(albumDB);
    QFile f(m3ufile);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "ERROR: Failed to open " << m3ufile;
        return NULL;
    }

    QFileInfo fi(m3ufile);
    QString name = fi.baseName();
    if (name.length() <= 0)
        name = "noname";

    TPPlaylist *playlist = NULL;
    QString artFileSmall, artFileBig, category;

    while (!f.atEnd())
    {
        QString line = QString::fromUtf8(f.readLine()).trimmed();

        if (line.length())
        {
            if (line.startsWith(M3UNAMETAG))
            {
                name = line.mid(M3UNAMETAG.length());
                if (name.length() <= 0)
                    name = "noname";
                else if (playlist)
                    playlist->setName(name);
            }
            else if (line.startsWith(M3UARTSMALLTAG))
            {
                artFileSmall = line.mid(M3UARTSMALLTAG.length());
            }
            else if (line.startsWith(M3UARTBIGTAG))
            {
                artFileBig = line.mid(M3UARTBIGTAG.length());
            }
            else if (line.startsWith(M3UCATEGORYTAG))
            {
                category = line.mid(M3UCATEGORYTAG.length());
            }
            else if (line[0] != '#')
            {
                if (!TPUtils::webAddress(line))
                {
                    TPTrack *track = trackDB->getTrackByFilename(line);
                    if (track)
                    {
                        if (!playlist)
                            playlist = new TPPlaylist(name, db, m3ufile);
                        playlist->add(track);
                    }
                    else
                        qDebug() << "track: " << line << " not found when constructing playlist";
                }
                else
                {
                    if (!playlist)
                        playlist = new TPPlaylist(name, db, m3ufile);

                    // This will create all stuff required to handle url: type of stuff.
                    playlist->add(line);
                    TPTrack *track = playlist->at(playlist->count()-1);
                    TPAlbum *album = track->getAlbum();
                    Q_ASSERT(album);

                    if (artFileBig.length())
                    {
                        album->setString(albumAttrArtLarge, TPPathUtils::getPlaylistArtFolder() + artFileBig);
                        playlist->setLargeArtName(artFileBig);
                    }
                    if (artFileSmall.length())
                    {
                        album->setString(albumAttrArtSmall, TPPathUtils::getPlaylistArtFolder() + artFileSmall);
                        playlist->setSmallArtName(artFileSmall);
                    }
                    if (!album->getString(albumAttrArtSmall).length() && album->getString(albumAttrArtLarge).length())
                        album->setString(albumAttrArtSmall, album->getString(albumAttrArtLarge));
                    if (album->getString(albumAttrArtSmall).length() && !album->getString(albumAttrArtLarge).length())
                        album->setString(albumAttrArtLarge, album->getString(albumAttrArtSmall));

                    if (category.length())
                        playlist->setCategory(category);

                 }
            }
        }
    }

    f.close();

    // Some special treats, for radio stations,
    // that basically contain just one track to some url.
    if (playlist && playlist->getCategory() == "radio")
    {
        for (int i=0;i<playlist->count();++i)
        {
            TPTrack *t = playlist->at(i);
            t->setString(objectAttrName, playlist->getName());
        }
    }

    return playlist;
}

bool TPPlaylistUtils::exportLocalPlaylist(TPPlaylist *playlist, QString m3ufile)
{
    Q_ASSERT(playlist);

    if (m3ufile.length() <= 0)
        // Not even planning to really save.
        return true;

    if (playlist->count())
    {
        if (m3ufile.length() <= 0)
            m3ufile = playlist->getFilename();
        else
            playlist->setFilename(m3ufile);

        QFile f(m3ufile);

        if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Failed to open " << m3ufile;
            return false;
        }

        f.write(M3UHEADER.toUtf8());
        f.write("\n");

        f.write(M3UNAMETAG.toUtf8());
        f.write(playlist->getName().toUtf8());
        f.write("\n");

        if (playlist->getCategory().length())
        {
            f.write(M3UCATEGORYTAG.toUtf8());
            f.write(playlist->getCategory().toUtf8());
            f.write("\n");
        }

        for (int i = 0; i < playlist->count(); ++i)
        {
            TPTrack *track = playlist->at(i);
            Q_ASSERT(track);

            QString line;
            line.append(track->getFilename());
            line.append("\n");
            f.write(line.toUtf8());
        }
        f.flush();
        f.close();
    }
    return true;
}
