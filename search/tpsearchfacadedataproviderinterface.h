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

#ifndef TPSEARCHFACADEDATAPROVIDERINTERFACE_H
#define TPSEARCHFACADEDATAPROVIDERINTERFACE_H

#include "tpassociative.h"
#include "search/operations/tpsearchfilteropinterface.h"
#include "tpstoredprocedureargs.h"

class TPSearchFacadeDataProviderInterface
{
protected:

    TPSearchFacadeDataProviderInterface(const QString _scheme)
    {
        scheme = _scheme;
        startIndex = 0;
    }

public:

    TPSearchFacadeDataProviderInterface()
    {
        Q_ASSERT("WRONG CONSTRUCTOR" == NULL);
    }

    virtual ~TPSearchFacadeDataProviderInterface(){;}

    const QString getScheme() const
    {
        return scheme;
    }

    inline void setStartIndex(int _startIndex)
    {
        startIndex = _startIndex;
    }

    inline TPAssociativeObject* getWithStartOffset(int index)
    {
        if (index < startIndex)
            // Not for us just yet.
            return NULL;
        index -= startIndex;
        if (index >= count())
            // Not for us either, something after us
            return NULL;

        return at(index);
    }

    virtual int count() = 0;

    virtual TPAssociativeObject* at(int index) = 0;

    virtual void configure(TPSearchFilterOpInterface */*rules*/, TPStoredProcedureArgs */*args*/)
    {

    }

private:

    QString scheme;
    int startIndex;
};

#endif // TPSEARCHFACADEDATAPROVIDERINTERFACE_H
