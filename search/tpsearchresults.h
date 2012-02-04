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

#ifndef TPSEARCHRESULTS_H
#define TPSEARCHRESULTS_H

#include <QVector>
#include <algorithm>
#include "tpsort.h"

class TPSearchResults
{
public:
    TPSearchResults(TPSortInterface *_sorter = 0, int estimatedCount = 4095);

    ~TPSearchResults()
    {
        qDeleteAll(results);
    }

    //! @brief Adds a new associative object to resulting list.
    void add(TPAssociativeObject *object);

    void sort();

    inline int count() const
    {
        return results.count();
    }

    inline const TPAssociativeObject& at(int index)
    {
        Q_ASSERT(index >= 0 && index < count());

        return results.at(index)->object();
    }

    void limitTo(int amount);

private:

    //! Sort interface that is to be used in comparison.
    TPSortInterface *sorter;

    //! Contains the search results if any.
    QVector< TPSortableAssociativeObject* > results;
};

#endif // TPSEARCHRESULTS_H
