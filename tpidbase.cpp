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

#include "tpidbase.h"
#include <QCryptographicHash>

TPIdBase::TPIdBase(const QByteArray _scheme)
{
    scheme = _scheme;
}

TPIdBase::TPIdBase(const QByteArray _scheme, const QString id1)
{
    scheme = _scheme;
    idSource1 = id1;
}

TPIdBase::TPIdBase(const QByteArray _scheme, const QString id1, const QString id2)
{
    scheme = _scheme;
    idSource1 = id1;
    idSource2 = id2;
}

void TPIdBase::addIdSource(const QString uniqueId)
{
    if (!idSource1.length())
        idSource1 = uniqueId;
    else if (!idSource2.length())
        idSource2 = uniqueId;
}

const QByteArray TPIdBase::identifier(bool includeScheme)
{
    if (!id.length())
    {
        Q_ASSERT(idSource1.length());

        QByteArray hashSource;
        hashSource += idSource1.toUtf8();
        hashSource += idSource2.toUtf8();
        QCryptographicHash hasher(QCryptographicHash::Md5);
        QByteArray hashResult = hasher.hash(hashSource, QCryptographicHash::Md5);

        id = hashResult.toHex();
    }

    if (includeScheme)
        return scheme + id;

    return id;
}

