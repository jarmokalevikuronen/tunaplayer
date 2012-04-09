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

#include "tpartist.h"
#include "tpalbum.h"
#include <QListIterator>
#include "tpschemes.h"
#include "tplog.h"

int TPArtist::instanceCount = 0;

TPArtist::TPArtist(QString _name, TPAssociativeDBItem *dbItem) : TPAssociativeObject(dbItem), TPIdBase(schemeArtist)
{
    ++instanceCount;

    setString(objectAttrName, _name);
    setString(objectAttrScheme, schemeArtist);
    addIdSource(_name);
    clearCachedValues();
}

TPArtist::~TPArtist()
{
    --instanceCount;

    if (!instanceCount)
        DEBUG() << "BASICTYPES: " << "Last Artist Deleted";

//    DEBUG() << "BASICTYPES: " << "~TPArtist";
    // Dereference all the albums and thus allow albums to be restroyed
    // if when required..
    QList<TPAlbum *>::iterator it = albums.begin();
    while (it != albums.end())
    {
        TPAlbum *album = *it;
        ++it;
        album->dec();
    }
}

QString TPArtist::getName() const
{
    return getString(objectAttrName);
}

int TPArtist::countAlbums() const
{
    return albums.size();
}

TPAlbum* TPArtist::getAlbum(int index) const
{
    Q_ASSERT(index >= 0 && index < countAlbums());

    return albums.at(index);
}

void TPArtist::addAlbum(TPAlbum *album)
{
    // Adds album, but only if not already present
    if (albums.indexOf(album) == -1)
    {
        albums.append(album);
        album->inc();
    }
}

int TPArtist::countTracks() const
{
    int count = 0;

    QListIterator<TPAlbum *> albumIterator(albums);
    while (albumIterator.hasNext())
    {
        TPAlbum *album = albumIterator.next();
        count += album->countTracks();
    }

    return count;
}
#if 1
void TPArtist::inc()
{
    TPReferenceCounted::inc();
}

void TPArtist::dec()
{
    TPReferenceCounted::dec();
}
#endif

QMap<QString, QVariant> TPArtist::toMap(QStringList *filteredKeys)
{
    QMap<QString, QVariant> values =
            TPAssociativeObject::toMap(filteredKeys);

    if (filteredKeys)
    {
        if (!filteredKeys->contains(artistAttrCountAlbums))
            values.insert(artistAttrCountAlbums, getInt(artistAttrCountAlbums));
        if (!filteredKeys->contains(artistAttrCountTracks))
            values.insert(artistAttrCountTracks, getInt(artistAttrCountTracks));
        if (!filteredKeys->contains(objectAttrPlayCount))
            values.insert(objectAttrPlayCount, getInt(objectAttrPlayCount));
        if (!filteredKeys->contains(objectAttrPlayLength))
            values.insert(objectAttrPlayLength, getInt(objectAttrPlayLength));
        if (!filteredKeys->contains(objectAttrLastPlayed))
            values.insert(objectAttrLastPlayed, getInt(objectAttrLastPlayed));
        if (!filteredKeys->contains(objectAttrIdentifier))
            values.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
    }
    else
    {
        values.insert(artistAttrCountAlbums, getInt(artistAttrCountAlbums));
        values.insert(artistAttrCountTracks, getInt(artistAttrCountTracks));
        values.insert(objectAttrPlayCount, getInt(objectAttrPlayCount));
        values.insert(objectAttrPlayLength, getInt(objectAttrPlayLength));
        values.insert(objectAttrLastPlayed, getInt(objectAttrLastPlayed));
        values.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
    }

    return values;
}

int TPArtist::getInt(const QString key, int defaultValue) const
{
    if (key == artistAttrCountAlbums)
        return albums.count();
    else if (key == artistAttrCountTracks)
    {
        int count = 0;
        for (int i=0;i<albums.count();++i)
            count += albums.at(i)->countTracks();
        return count;
    }
    else if (key == objectAttrPlayCount)
    {
        if (cache.playCount >= 0)
            return cache.playCount;

        cache.playCount = 0;
        for (int i=0;i<albums.count();++i)
            cache.playCount += albums.at(i)->getInt(objectAttrPlayCount);
        return cache.playCount;
    }
    else if (key == objectAttrPlayLength)
    {
        if (cache.playLen >= 0)
            return cache.playLen;

        cache.playLen = 0;
        for (int i=0;i<albums.count();++i)
            cache.playLen += albums.at(i)->getInt(objectAttrPlayLength);
        return cache.playLen;
    }
    else if (key == objectAttrLastPlayed)
    {
        if (cache.lastPlayed >= 0)
            return cache.lastPlayed;

        cache.lastPlayed = 0;
        for (int i=0;i<albums.count();++i)
        {
            int tmpTs = albums.at(i)->getInt(objectAttrLastPlayed);
            cache.lastPlayed = qMax(cache.lastPlayed, tmpTs);
        }
        return cache.lastPlayed;
    }
    return TPAssociativeObject::getInt(key, defaultValue);
}

const QString TPArtist::getString(const QString key, const QString defaultValue) const
{
    if (key == objectAttrIdentifier)
        return QString(const_cast<TPArtist *>(this)->identifier());
    else if (key == objectAttrUserTokens_DYNAMIC)
    {
        QStringList tokens;

        // Here, the ARTIST should be shown with all the users that has
        // configured to show at least one album from this particular artist.
        foreach(TPAlbum *album, albums)
        {
            QString toks = album->getString(objectAttrUserTokens_DYNAMIC);
            if (toks.length())
            {
                QStringList albumTokens = toks.split(userTokensDelimiter);
                tokens.append(albumTokens);
                tokens.removeDuplicates();
            }
        }

//        DEBUG() << "TOKENS(artist:" << getString(objectAttrName) << ") = " << tokens;

        return tokens.join(userTokensDelimiter);
    }

    return TPAssociativeObject::getString(key, defaultValue);
}

void TPArtist::clearCachedValues()
{
    TPAssociativeObject::clearCachedValues();
    cache.clear();
}


