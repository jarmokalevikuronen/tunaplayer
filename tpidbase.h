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

#ifndef TPIDBASE_H
#define TPIDBASE_H

#include <QString>
#include <QByteArray>

class TPIdBase
{
public:
    TPIdBase(const QByteArray scheme);
    TPIdBase(const QByteArray scheme, const QString id1);
    TPIdBase(const QByteArray scheme, const QString id1, const QString id2);

    const QByteArray identifier(bool includeScheme = true);
    const QByteArray getScheme() const
    {
        return scheme;
    }

protected:

    void addIdSource(const QString uniqueId);

private:

    QByteArray id;
    QByteArray scheme;
    QString idSource1;
    QString idSource2;
};

#endif // TPIDBASE_H
