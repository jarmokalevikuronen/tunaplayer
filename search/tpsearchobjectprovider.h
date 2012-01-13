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

#ifndef TPSEARCHOBJECTPROVIDER_H
#define TPSEARCHOBJECTPROVIDER_H

#include "tpassociative.h"
#include "tpsearchfacadedataproviderinterface.h"
#include "search/operations/tpsearchfilteropinterface.h"
#include "tpstoredprocedureargs.h"
#include <QVariantList>

class TPSearchObjectProvider
{
public:

    TPSearchObjectProvider()
    {
    }

    ~TPSearchObjectProvider()
    {
        qDeleteAll(providers);
    }

    //! @brief Sets a list of schemes to whom the count+at should apply.
    //! Clears possibly previously set list.
    void setSchemes(QVariantList listOfSchemes, TPSearchFilterOpInterface *selector, TPStoredProcedureArgs *args);

    //! amount of items this facade providers
    int count();

    //! getter for an item.
    TPAssociativeObject* at(int index);

    //! @brief Adds a data provider. Ownership transfers.
    void addProvider(TPSearchFacadeDataProviderInterface *provider);

private:

    QVector< TPSearchFacadeDataProviderInterface *> providers;
    QVector< TPSearchFacadeDataProviderInterface *> selectedProviders;

};

#endif // TPSEARCHOBJECTPROVIDER_H
