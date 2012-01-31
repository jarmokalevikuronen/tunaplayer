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

#ifndef TPSTOREDPROCEDURE_H__
#define TPSTOREDPROCEDURE_H__

#include <QString>
#include <QVariantMap>
#include <QVariantList>
#include <QFile>
#include <QDebug>
#include <QtGlobal>
#include <QStringList>
#include "tpstoredprocedureargs.h"
#include "sort/sorting.h"
#include "operations/tpsearchfilteropfactory.h"

class TPStoredProcedure
{
public:

    TPStoredProcedure();
    ~TPStoredProcedure();

    //! Compiles the stored procedure specified by the file.
    //! returns true if succeeded, false otherwise.
    bool compile(const QString _filename);

    const QString getErrorDescription();

    inline QVariantList getSchemes() const
    {
        return schemes;
    }

    inline TPSearchFilterOpInterface* getFilter() const
    {
        return rootFilter;
    }

    inline TPSortInterface* getSorter() const
    {
        return rootSorter;
    }

    inline QStringList* getFilterKeys() const
    {
        return filterKeys;
    }

    inline QString getGroupBy() const
    {
        return groupBy;
    }

    inline TPSearchFilterOpInterface* getProviderSelector() const
    {
        return rootProviderSelector;
    }

    TPStoredProcedureArgs* createArguments(QVariantMap receivedOverrideArguments) const;

private:

    bool processFilters(QVariantMap varMap);
    bool processProviderSelectors(QVariantMap varMap);
    bool processFilterItem(QString key, QVariantMap subElements);
    bool processFilterComparison(QVariantList subElements);
    bool processSorting(const QVariantList sortList);
    bool processSchemes(QVariantList varList);
    bool processArguments(QVariantMap varMap);

    bool parseContainerFilter(TPSearchFilterOpInterface *parent, const QVariantMap argMap);
    bool parseMatchFilter(TPSearchFilterOpInterface *parent, const QVariantList argList);

private:

    //! Root filter under which all other filters are
    //! assigned.
    TPSearchFilterOpInterface* rootFilter;

    //! This is used to configure the actual
    //! Data provider before selections are to be made.
    TPSearchFilterOpInterface* rootProviderSelector;

    //! Sorter inteface, if any.
    TPSortInterface *rootSorter;

    //! List of schemes
    QVariantList schemes;

    //! Contains the arguments extract from the JSON document.
    QVariantMap arguments;

    //! The original JSON stuff.
    QVariant json;

    //! Filename where this was built from.
    QString filename;

    //! Contains a description of the error that has occured.
    QString errorDescription;

    //! Keys to be filtered from the result set.
    QStringList* filterKeys;

    //! Grouping by a specific key if any.
    //! Please note that this is required to be present
    //! in all the objects being processed otherwise
    //! will not be applied at all.
    QString groupBy;
};

#endif // TPSTOREDPROCEDURE_H
