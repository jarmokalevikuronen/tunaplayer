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

#include "tpstoredprocedure.h"
#include "tpsort.h"
#include "json.h"
#include "tplog.h"
#include "tpsortinterfacefactory.h"

using namespace QtJson;

static const QString spArguments("arguments");
static const QString spSchemes("schemes");
static const QString spProviders("providers");
static const QString spFilters("filters");
static const QString spProviderSelectors("provider-selectors");
static const QString spSorting("sorting");
static const QString spFilterKeys("filterkeys");
static const QString spGroupBy("grouping-by");
static const QString spProviderConfig("provider-config");

TPStoredProcedure::TPStoredProcedure()
{
    rootFilter = NULL;
    rootSorter = NULL;
    filterKeys = NULL;
    rootProviderSelector = NULL;
}

TPStoredProcedure::~TPStoredProcedure()
{
    delete rootFilter;
    delete rootSorter;
    delete filterKeys;
    delete rootProviderSelector;
}

bool TPStoredProcedure::compile(const QString _filename)
{
    filename = _filename;

    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
    {
        WARN() << "TSP: Failed to open " << filename;
        return false;
    }
    QByteArray fileContent = f.readAll();

    if (!fileContent.length())
    {
        WARN() << "TSP: Empty file: " << filename;
        return false;
    }

    bool ok = false;
    QVariant v = Json::parse(fileContent, ok);

    if (!ok)
    {
        WARN() << "Failed to parse JSON from " << filename;
        return false;
    }

    bool success = true;
    QVariantMap vm = v.toMap();
    QVariantMap::iterator it = vm.begin();
    while (success && it != vm.end())
    {
        if (it.key() == spArguments)
            success = processArguments(it.value().toMap());
        else if (it.key() == spFilters)
            success = processFilters(it.value().toMap());
        else if (it.key() == spSorting)
            success = processSorting(it.value().toList());
        else if (it.key() == spSchemes || it.key() == spProviders)
            success = processSchemes(it.value().toList());
        else if (it.key() == spProviderSelectors)
            success = processProviderSelectors(it.value().toMap());
        else if (it.key() == spFilterKeys)
        {
            QVariantList vl = it.value().toList();
            if (vl.count())
            {
                delete filterKeys; filterKeys = 0;
                filterKeys = new QStringList;
                QVariantList::iterator it = vl.begin();
                while (it != vl.end())
                {
                    (*filterKeys).append(it->toString());
                    ++it;
                }
            }
        }
        else if (it.key() == spGroupBy)
            groupBy = it.value().toString();
        else
            ERROR() << "TSP: Unk. item: " << it.key() << "=" << it.value();

        it++;
    }

    if (!success)
    {
        ERROR() << "TSP: Failed to parse sp from: " << filename;
        return false;
    }

    return true;
}

TPStoredProcedureArgs* TPStoredProcedure::createArguments(QVariantMap receivedOverrideArguments) const
{
    // Copy the template
    QVariantMap patched = arguments;
    // Patch with received set of arguments
    QVariantMap::iterator it = receivedOverrideArguments.begin();
    while (it != receivedOverrideArguments.end())
    {
        patched[it.key()] = it.value();
        it++;
    }

    // Test that there exists not arguments that have empty value -> considered as an error
    it = patched.begin();
    while (it != patched.end())
    {
        QVariant value = it.value();

        if (value.isNull() || !value.isValid())
            return NULL;
        else if (value.toString().length() < 1)
            return NULL;
        it++;
    }

    return new TPStoredProcedureArgs(patched);
}

bool TPStoredProcedure::processSchemes(QVariantList varList)
{
    schemes = varList;
    return varList.count() > 0;
}

bool TPStoredProcedure::processArguments(QVariantMap varMap)
{
    arguments = varMap;
    return true;
}

bool TPStoredProcedure::parseContainerFilter(TPSearchFilterOpInterface *parent, const QVariantMap varMap)
{
    QVariantMap::const_iterator it = varMap.begin();
    while (it != varMap.end())
    {
        TPSearchFilterOpInterface *op =
                TPSearchFilterOpFactory::create(it.key());
        if (!op)
        {
            errorDescription.append("Failed to create %1").arg(it.key());
            return false;
        }

        parent->add(op);
        if (op->isContainer())
        {
            // Recursive call here..
            if (!parseContainerFilter(op, it.value().toMap()))
                return false;
        }
        else
        {
            if (!parseMatchFilter(op, it.value().toList()))
                return false;
        }

        it++;
    }

    return varMap.count() > 0;
}

bool TPStoredProcedure::parseMatchFilter(TPSearchFilterOpInterface *parent, const QVariantList argList)
{
    if (argList.count() != 2)
    {
        errorDescription.append("Unexpected amount of arguments for match %1").arg(argList.count());
        return false;
    }

    if (!parent->addArg(argList.at(0).toString()))
    {
        errorDescription.append("Failed to parse %1").arg(argList.at(0).toString());
        return false;
    }

    if (!parent->addArg(argList.at(1).toString()))
    {
        errorDescription.append("Failed to parse %1").arg(argList.at(1).toString());
        return false;
    }

    return true;
}

bool TPStoredProcedure::processProviderSelectors(QVariantMap varMap)
{
    if (varMap.count() != 1)
    {
        errorDescription = "Expected only one root element for Filters";
        return false;
    }

    rootProviderSelector = TPSearchFilterOpFactory::create(varMap.begin().key());
    if (!rootProviderSelector)
    {
        errorDescription = "Failed to create " + varMap.begin().key();
        return false;
    }

    if (rootProviderSelector->isContainer())
        return parseContainerFilter(rootProviderSelector, varMap.begin().value().toMap());
    else
        return parseMatchFilter(rootProviderSelector, varMap.begin().value().toList());
}

bool TPStoredProcedure::processFilters(QVariantMap varMap)
{
    if (varMap.count() != 1)
    {
        errorDescription = "Expected only one root element for Filters";
        return false;
    }

    rootFilter = TPSearchFilterOpFactory::create(varMap.begin().key());
    if (!rootFilter)
    {
        errorDescription = "Failed to create " + varMap.begin().key();
        return false;
    }

    if (rootFilter->isContainer())
        return parseContainerFilter(rootFilter, varMap.begin().value().toMap());
    else
        return parseMatchFilter(rootFilter, varMap.begin().value().toList());
}

bool TPStoredProcedure::processSorting(const QVariantList sortList)
{
    for (int i=0;i<sortList.count();++i)
    {
        QVariantList sortItem = sortList.at(i).toList();
        if (sortItem.count() != 2)
        {
            errorDescription.append("Unexpected amount of sort items: %1").arg(sortItem.count());
            return false;
        }
        QString order = sortItem.at(0).toString();
        QString key = sortItem.at(1).toString();

        TPSortInterface *sif = TPSortInterfaceFactory::create(order, key);
        if (!sif)
        {
            errorDescription.append("Failed to create sorter from order=%1 key=%2").arg(order).arg(key);
            return false;
        }

        if (!rootSorter)
            rootSorter = sif;
        else
            rootSorter->addToChain(sif);
    }

    return true;
}

const QString TPStoredProcedure::getErrorDescription()
{
    QString err = errorDescription;
    errorDescription.clear();
    return err;
}
