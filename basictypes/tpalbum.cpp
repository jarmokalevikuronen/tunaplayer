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

#include "tpalbum.h"
#include "tpschemes.h"
#include "tplog.h"

int TPAlbum::instanceCount = 0;

TPAlbum::TPAlbum(TPArtist *_artist, QString _name, TPAssociativeDBItem *dbItem) : TPAssociativeObject(dbItem), TPIdBase(schemeAlbum)
{
    ++instanceCount;
//    DEBUG() << "BASICTYPES: TPAlbum (instances now: " << instanceCount << ", object=" << this << ")";

    Q_ASSERT(_artist);
    artist = _artist;
    setString(objectAttrName, _name);
    setString(objectAttrScheme, schemeAlbum);
    artist->addAlbum(this);
    year = AlbumYearNotResolved;

    addIdSource(artist->getName() + "\n");
    addIdSource(_name);
}

TPAlbum::TPAlbum(QString _name, TPAssociativeDBItem *dbItem) : TPAssociativeObject(dbItem), TPIdBase(schemeAlbum)
{
    ++instanceCount;
 //   DEBUG() << "BASICTYPES: TPAlbum (instances now: " << instanceCount << ", object=" << this << ")";

    artist = NULL;
    year = AlbumYearNotResolved;
    setString(objectAttrName, _name);
    setString(objectAttrScheme, schemeAlbum);
    addIdSource(_name);
}

TPAlbum::TPAlbum(QString _name) : TPAssociativeObject(_name), TPIdBase(schemeAlbum)
{
    ++instanceCount;
//    DEBUG() << "BASICTYPES: TPAlbum (instances now: " << instanceCount << ", object=" << this << ")";

    artist = NULL;
    year = AlbumYearNotResolved;
    setString(objectAttrName, _name);
    setString(objectAttrScheme, schemeAlbum);
    addIdSource(_name);
}

TPAlbum::~TPAlbum()
{
    --instanceCount;
    if (!instanceCount)
        DEBUG() << "BASICTYPES" << "Last Album Deleted";
 //   DEBUG() << "BASICTYPES: " << "~TPAlbum (instances remain: " << instanceCount << ", object=" << this << ")";

    // Dereference from all the possible tracks we are currently referencing.
    QList<TPTrack *>::iterator it = tracks.begin();
    while (it != tracks.end())
    {
        TPTrack *track = *it;
        ++it;
        track->dec();
    }
}

TPArtist* TPAlbum::getArtist() const
{
    return artist;
}

QString TPAlbum::getName() const
{
    return getString(objectAttrName);
}

void TPAlbum::addTrack(TPTrack *track)
{
    Q_ASSERT(track);

    QList<TPTrack *>::iterator it = tracks.begin();
    while (it != tracks.end())
    {
        TPTrack *curTrack = *it;
        if (curTrack->getIndex() > track->getIndex())
        {
            track->inc();
            tracks.insert(it, track);
            return;
        }
        ++it;
    }
    track->inc();
    tracks.append(track);
}

void TPAlbum::unlinkTrack(TPTrack *track)
{
    int indexOfTrack = tracks.indexOf(track);
    Q_ASSERT(indexOfTrack >= 0);

    // Remove from array and dereference the track object.
    tracks.removeAt(indexOfTrack);
    track->dec();
}

TPTrack* TPAlbum::getTrack(int index) const
{
    Q_ASSERT(index >= 0);

    QList<TPTrack *>::const_iterator it = tracks.constBegin();
    while (it != tracks.constEnd())
    {
        if ((*it)->getIndex() == index)
            return *it;
        ++it;
    }


    return NULL;
}

TPTrack* TPAlbum::getTrackByPos(int pos) const
{
    Q_ASSERT(pos >= 0 && pos < tracks.size());

    return tracks.at(pos);
}

int TPAlbum::countTracks() const
{
    return tracks.size();
}

int TPAlbum::lastTrack() const
{
    if (tracks.size())
        tracks.last()->getIndex();

    return -1;
}

bool TPAlbum::isComplete() const
{
    return lastTrack() == (countTracks()+1);
}

QSet<int> TPAlbum::getMissingTracks() const
{
    if (isComplete())
        return QSet<int>();

    QSet<int> missingTracks;

    int count = countTracks();

    for (int i = 1; i <= count; ++i)
        if (!getTrack(i))
            missingTracks.insert(i);

    return missingTracks;
}

bool TPAlbum::isEmpty() const
{
    return countTracks() <= 0;
}

int TPAlbum::getLen() const
{
    int len = 0;

    QList<TPTrack *>::const_iterator it = tracks.constBegin();
    while (it != tracks.constEnd())
    {
        len += (*it)->getLen();
        ++it;
    }
    return len;
}

int TPAlbum::getYear() const
{
    int resolvedYear = -1;

    if (year == AlbumYearNotResolved)
    {
        QList<TPTrack *>::const_iterator it = tracks.constBegin();
        while (it != tracks.constEnd())
        {
            TPTrack *track = *it;
            int testYear = track->getYear();
            if (resolvedYear == -1)
                resolvedYear = testYear;
            else if (resolvedYear != testYear)
            {
                year = AlbumYearNotAvailable;
                return year;
            }

            ++it;
        }

        // Make sure resolved year is somewhat rational
        if (resolvedYear < 1900 || resolvedYear > 2100)
            year = AlbumYearNotAvailable;
        else
            year = resolvedYear;
    }

    return year;
    }
#if 1
void TPAlbum::inc()
{
    TPReferenceCounted::inc();
//    DEBUG() << "BASICTYPES: TPAlbum " << this << " references++: " << counter;

}

void TPAlbum::dec()
{
    TPReferenceCounted::dec();
 //   DEBUG() << "BASICTYPES: TPAlbum " << this << " references--: " << counter;
}
#endif

QMap<QString, QVariant> TPAlbum::toMap(QStringList *filteredKeys)
{
    QMap<QString, QVariant> values =
            TPAssociativeObject::toMap(filteredKeys);

    bool smallArt = hasAlbumArt(ESmallArt);
    bool largeArt = hasAlbumArt(EBigArt);

    // Make sure possible existing definitions are removed in case
    // they were defined but the actual art file is not in disk
    if (!smallArt)
        values.remove(albumAttrArtSmall);
    if (!largeArt)
        values.remove(albumAttrArtLarge);

    // Insert also aggregated information.
    if (filteredKeys)
    {
        if (!filteredKeys->contains(albumAttrCountTracks))
            values.insert(albumAttrCountTracks, getInt(albumAttrCountTracks));
        if (!filteredKeys->contains(objectAttrPlayCount))
            values.insert(objectAttrPlayCount, getInt(objectAttrPlayCount));
        if (!filteredKeys->contains(objectAttrPlayLength))
            values.insert(objectAttrPlayLength, getInt(objectAttrPlayLength));
        if (!filteredKeys->contains(objectAttrLastPlayed))
            values.insert(objectAttrLastPlayed, getInt(objectAttrLastPlayed));
        if (!filteredKeys->contains(albumAttrArtistName))
            values.insert(albumAttrArtistName, getString(albumAttrArtistName));
        if (!filteredKeys->contains(albumAttrArtistId))
            values.insert(albumAttrArtistId, getString(albumAttrArtistId));
        if (!filteredKeys->contains(objectAttrIdentifier))
            values.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
        if (!filteredKeys->contains(albumAttrArtSmall))
        {
            if (smallArt)
                values.insert(albumAttrArtSmall, TPPathUtils::getNormalizedPathForWebServer(fullPathToAlbumArt(ESmallArt)));
        }
        if (!filteredKeys->contains(albumAttrArtLarge))
        {
            if (largeArt)
                values.insert(albumAttrArtLarge, TPPathUtils::getNormalizedPathForWebServer(fullPathToAlbumArt(EBigArt)));
        }
    }
    else
    {
        values.insert(albumAttrCountTracks, getInt(albumAttrCountTracks));
        values.insert(objectAttrPlayCount, getInt(objectAttrPlayCount));
        values.insert(objectAttrPlayLength, getInt(objectAttrPlayLength));
        values.insert(objectAttrLastPlayed, getInt(objectAttrLastPlayed));
        values.insert(albumAttrArtistName, getString(albumAttrArtistName));
        values.insert(albumAttrArtistId, getString(albumAttrArtistId));
        values.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
        if (smallArt)
            values.insert(albumAttrArtSmall, TPPathUtils::getNormalizedPathForWebServer(fullPathToAlbumArt(ESmallArt)));
        if (largeArt)
            values.insert(albumAttrArtLarge, TPPathUtils::getNormalizedPathForWebServer(fullPathToAlbumArt(EBigArt)));
    }

    return values;
}

const QString TPAlbum::getString(const QString key, const QString defaultValue) const
{
    if (key.startsWith(albumAttrArtist))
        return artist ? artist->getString(key.mid(albumAttrArtist.length())) : QString("");
    else if (key == objectAttrIdentifier)
        return QString(const_cast<TPAlbum*>(this)->identifier());
    else if (artist)
    {
        if (key == albumAttrArtistName)
            return artist->getString(objectAttrName);
        else if (key == albumAttrArtistId)
            return artist->getString(objectAttrIdentifier);
    }

/*    if (TPAssociativeObject::contains(key))
        return TPAssociativeObject::getString(key, defaultValue);

    //
    // Could be numeric aggregated value available through getInt
    //
    return QString::number(getInt(key));*/
    return TPAssociativeObject::getString(key, defaultValue);
}


int TPAlbum::getInt(const QString key, int defaultValue) const
{
    if (key == albumAttrCountTracks)
        return tracks.count();
    else if (key == objectAttrLen)
    {
        int length = 0;
        for (int i=0;i<tracks.count();++i)
            length += tracks.at(i)->getLen();
        return length;
    }
    else if (key == objectAttrPlayCount)
    {
        int count = 0;
        for (int i=0;i<tracks.count();++i)
            count += tracks.at(i)->getInt(objectAttrPlayCount);
        return count;
    }
    else if (key == objectAttrPlayLength)
    {
        int length = 0;
        for (int i=0;i<tracks.count();++i)
            length += tracks.at(i)->getInt(objectAttrPlayLength);
        return length;
    }
    else if (key == objectAttrLastPlayed)
    {
        int ts = 0;
        for (int i=0;i<tracks.count();++i)
        {
            int tmpTs = tracks.at(i)->getInt(objectAttrLastPlayed);
            ts = qMax(ts, tmpTs);
        }
        return ts;
    }

    // Fallback to non-aggregated values...
    return TPAssociativeObject::getInt(key, defaultValue);
}
