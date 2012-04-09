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

#include <QUrl>
#include <QStringList>
#include <QDebug>
#include "tptrack.h"
#include "tpgenre.h"
#include "tpalbum.h"
#include "tpschemes.h"
#include "tplog.h"

int TPTrack::instanceCount = 0;

TPTrack::TPTrack(TPAlbum *_album, TPAssociativeDBItem *_item)
    : TPAssociativeObject(_item), TPIdBase(schemeTrack)
{
    ++instanceCount;

//    DEBUG() << "BASICTYPES: " << "TPTrack (instances now: " << instanceCount << ", object=" << this << ")";

    idGenerated = false;

    Q_ASSERT(_album);
    Q_ASSERT(_item);

    album = _album;
    album->inc();
    album->addTrack(this);

    setString(objectAttrScheme, schemeTrack);
}

TPTrack::TPTrack(TPAlbum *_album) : TPIdBase(schemeTrack)
{
    ++instanceCount;

//    DEBUG() << "BASICTYPES: " << "TPTrack (instances now: " << instanceCount << ", object=" << this << ")";

    idGenerated = false;

    Q_ASSERT(_album);

    album = _album;
    album->inc();
    album->addTrack(this);

    setString(objectAttrScheme, schemeTrack);
}

TPTrack::~TPTrack()
{
    --instanceCount;

    if (!instanceCount)
        DEBUG() << "BASICTYPES: " << "Last Track Deleted";
//    DEBUG() << "BASICTYPES: " << "~TPTrack (instances remain: " << instanceCount << ")";

    detachFromAlbum();

    // If we are acting as a delegate (for example
    // representing a feed item during playback),
    // do detach from the owner.
    if (delegate.ao)
        delegate.ao->save(0);

    if (delegate.rc)
        delegate.rc->dec();
}

void TPTrack::detachFromAlbum()
{
    if (album)
    {
        album->unlinkTrack(this);
        album->dec();
        album = NULL;
    }
}

#if 1
void TPTrack::inc()
{
    TPReferenceCounted::inc();
//    DEBUG() << "BASICTYPES: Track " << this << " references++: " << counter;
}

void TPTrack::dec()
{
    TPReferenceCounted::dec();
//    DEBUG() << "BASICTYPES: Track " << this << " references--: " << counter;
}
#endif

const QString TPTrack::getString(const QString key, const QString defaultValue) const
{
    if (key.startsWith(trackAttrAlbum))
        return album ? album->getString(key.mid(trackAttrAlbum.length())) : QString("");
    else if (key == trackAttrAlbumId)
        return album ? album->getString(objectAttrIdentifier) : QString("");
    else if (key == trackAttrArtistId)
        return album && album->getArtist() ? album->getArtist()->getString(objectAttrIdentifier) : QString("");
    else if (key == objectAttrIdentifier)
        return const_cast<TPTrack *>(this)->identifier();

    return TPAssociativeObject::getString(key, defaultValue);
}

QVariantMap TPTrack::toMap(QStringList *filteredKeys)
{
    QMap<QString, QVariant> props;

    props = TPAssociativeObject::toMap(filteredKeys);

    if (filteredKeys)
    {
        if (!filteredKeys->contains(trackAttrAlbumId))
            props.insert(trackAttrAlbumId, getString(trackAttrAlbumId));
        if (!filteredKeys->contains(objectAttrIdentifier))
            props.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
        if (!filteredKeys->contains(trackAttrArtistId))
            props.insert(trackAttrArtistId, getString(trackAttrArtistId));
        if (!filteredKeys->contains(albumAttrArtLarge))
            props.insert(albumAttrArtLarge, getString(albumAttrArtLarge));
    }
    else
    {
        props.insert(trackAttrAlbumId, getString(trackAttrAlbumId));
        props.insert(objectAttrIdentifier, getString(objectAttrIdentifier));
        props.insert(trackAttrArtistId, getString(trackAttrArtistId));
        props.insert(albumAttrArtLarge, getString(albumAttrArtLarge));
    }

    return props;
}

