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

#include "tpsearchobjectprovider.h"
#include <QVariantList>

void TPSearchObjectProvider::setSchemes(QVariantList listOfSchemes, TPSearchFilterOpInterface *selector, TPStoredProcedureArgs *arguments)
{
    selectedProviders.clear();

    for (int i=0;i<listOfSchemes.count();++i)
    {
        QString scheme = listOfSchemes.at(i).toString();
        for (int i=0;i<providers.count();++i)
        {
            TPSearchFacadeDataProviderInterface *provider = providers.at(i);
            if (provider->getScheme() == scheme)
            {
                if (selectedProviders.indexOf(provider) < 0)
                    selectedProviders.append(provider);
            }
        }
    }

    // Configure the selected providers accordingly..
    for (int i=0;i<selectedProviders.count();++i)
        selectedProviders.at(i)->configure(selector, arguments);

    // Precalculate start indexes for faster access in ::at method.
    int startOffset = 0;
    for (int i=0;i<selectedProviders.count();++i)
    {
        selectedProviders.at(i)->setStartIndex(startOffset);
        startOffset += selectedProviders.at(i)->count();
    }
}

int TPSearchObjectProvider::count()
{
    int count = 0;

    for (int i=0;i<selectedProviders.count();++i)
        count += selectedProviders.at(i)->count();

    return count;
}

TPAssociativeObject* TPSearchObjectProvider::at(int index)
{
    TPAssociativeObject *object = NULL;

    for (int i=0;i<selectedProviders.count();++i)
    {
        object = selectedProviders.at(i)->getWithStartOffset(index);
        if (object)
            break;
    }

    return object;
}

void  TPSearchObjectProvider::addProvider(TPSearchFacadeDataProviderInterface *provider)
{
    if (providers.indexOf(provider) < 0)
        providers.append(provider);
}

