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

#ifndef TPSORTIMPLEMENTATIONSTRING_H
#define TPSORTIMPLEMENTATIONSTRING_H

#include <QtCore>
#include "tpsortimplementationstringbase.h"

class TPSortImplementationString : public TPSortImplementationStringBase
{
public:

    TPSortImplementationString(Qt::CaseSensitivity _cs, bool _ascending, const QString _key);

private:

    bool lessThan(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const;
    bool equals(const TPSortableAssociativeObject &object1, const TPSortableAssociativeObject &object2) const;

private:

    bool ascending;
    Qt::CaseSensitivity cs;
};

#endif // TPSORTIMPLEMENTATIONSTRING_H
