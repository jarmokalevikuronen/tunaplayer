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

#ifndef TRACK_H
#define TRACK_H

#include <QString>
#include <QFile>
#include "tpgenre.h"
#include "tpreferencecounted.h"
#include "tpassociative.h"
#include "tpassociativemeta.h"
#include "tpidbase.h"

class TPAlbum;

class TPObjectDelegate
{
public:

    TPObjectDelegate(TPAssociativeObject *_ao = 0, TPReferenceCounted *_rc = 0)
    {
        rc = _rc;
        ao = _ao;
    }

    TPAssociativeObject *ao;
    TPReferenceCounted *rc;
};

class TPTrack :
        public TPReferenceCounted,
        public TPAssociativeObject,
        public TPIdBase
{
public:

    TPTrack(TPAlbum *_album);
    TPTrack(TPAlbum *_album, TPAssociativeDBItem *dbItem);

    const QString getString(const QString key, const QString defaultValue="") const;
    QMap<QString, QVariant> toMap(QStringList *filteredKeys);


    ~TPTrack();

    void setActingAsDelegateOf(TPObjectDelegate _delegate)
    {
        if (delegate.rc)
            delegate.rc->dec();

        delegate = _delegate;

        if (delegate.rc)
            delegate.rc->inc();
    }

    int getIndex() const
    {
        return getInt(trackAttrIndex);
    }

    QString getName() const
    {
        return getString(objectAttrName);
    }

    QString getFilename() const
    {
        return getString(trackAttrFilename);
    }

    QString getGenre() const
    {
        return getString(trackAttrGenre);
    }

    TPAlbum* getAlbum() const
    {
        return album;
    }

    int getLen() const
    {
        return getInt(objectAttrLen, -1);
    }

    void setLen(int len)
    {
        // This does not trigger the db saving op.
        setInt(objectAttrLen, len);
    }

    int getYear() const
    {
        return getInt(trackAttrYear);
    }

    void incPlayCount()
    {
        if (delegate.ao)
        {
            delegate.ao->incIntValue(objectAttrPlayCount);
            delegate.ao->save(1000);
        }
        else
        {
            incIntValue(objectAttrPlayCount);
            save(1000);
        }
    }

    const QByteArray identifier(bool includeScheme = true)
    {
        if (!idGenerated)
        {
            addIdSource(getFilename());
            // We will save the object identifier into the map of information held.
            setByteArray(objectAttrIdentifier, TPIdBase::identifier(false));
            idGenerated = true;
        }

        return TPIdBase::identifier(includeScheme);
    }

    void detachFromAlbum();
#if 1
    void inc();
    void dec();
#endif
private:

    //!
    bool idGenerated;

    //! Album this track is part of. Can be empty in case of streamed file.
    TPAlbum* album;

    // This class might act as a delegate of some
    // other object and should redirect certain calls
    // to the original object instead.
    TPObjectDelegate delegate;
};

#endif // TRACK_H
