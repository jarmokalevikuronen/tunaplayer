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

#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QVariant>
#include "tpassociative.h"
#include "tpassociativemeta.h"
#include "tppathutils.h"
#include "tplog.h"
#include "tputils.h"

TPAssociativeObject::TPAssociativeObject(const QString primaryKey)
{
    item = new TPAssociativeDBItem(primaryKey);
    itemOwned = true;
    TPAssociativeObject::clearCachedValues();
}


TPAssociativeObject::TPAssociativeObject(TPAssociativeDBItem *_item)
{
    itemOwned = false;
    item = _item;
    TPAssociativeObject::clearCachedValues();
}

TPAssociativeObject::~TPAssociativeObject()
{
    if (itemOwned)
        delete item;
    item = 0;
}

void TPAssociativeObject::setInt(const QString key, int value)
{
    if (item)
        item->setIntValue(key, value);
}

void TPAssociativeObject::incIntValue(const QString key, int by)
{
    if (item)
        item->incIntValue(key, by);
}

const QVariant TPAssociativeObject::get(const QString key) const
{
    if (item)
        return item->value(key);

    return QVariant();
}

int TPAssociativeObject::getInt(const QString key, int defaultValue) const
{
    //
    // Special attribute handling here - basically
    // aggregated value calculated from current time are placed here.
    //
    if (key == objectAttrAge)
    {
        if (cachedAge >= 0 && cachedAge != INT_MAX)
            return cachedAge;

        int created = getInt(objectAttrCreated, -1);
        if (created > 0)
        {
            cachedAge = TPUtils::currentEpoch() - created;

            // Deal with clock skews..
            if (cachedAge < 0)
                cachedAge = 0;
        }
        else
            cachedAge = INT_MAX;

        return cachedAge;
    }
    else if (key == objectAttrLastPlayedAgo)
    {
        if (cachedLastPlayedAgo >= 0 && cachedLastPlayedAgo != INT_MAX)
            return cachedLastPlayedAgo;

        int lastplt = getInt(objectAttrLastPlayed, -1);
        if (lastplt >= 0)
        {
            cachedLastPlayedAgo = TPUtils::currentEpoch() - lastplt;

            // Deal with clock skew.
            if (cachedLastPlayedAgo < 0)
                cachedLastPlayedAgo = 0;
        }
        else
            cachedLastPlayedAgo = INT_MAX;

        return cachedLastPlayedAgo;
    }

    if (item)
        return item->intValue(key, defaultValue);

    return defaultValue;
}

void TPAssociativeObject::setString(const QString key, const QString value)
{
    if (item)
        item->setStringValue(key, value);
}

void TPAssociativeObject::setByteArray(const QString key, const QByteArray value)
{
    if (item)
        item->setValue(key, value);
}

const QString TPAssociativeObject::getString(const QString key, const QString defaultValue) const
{
    if (contains(key))
        return item->value(key).toString();

    // Very much hack here. Should be able to query (optional param etc.) whether the
    // value is actually available as this effectively disallows using of negative
    // values as integers (not that there would be really be need to use those as such).
    int integerValue = getInt(key, INT_MIN+1);
    if (integerValue != INT_MIN+1)
        return QString::number(integerValue);

    return defaultValue;
}

bool TPAssociativeObject::save(int timeoutMs)
{
    if (item)
        return item->save(timeoutMs);
    return false;
}


QMap<QString, QVariant> TPAssociativeObject::toMap(QStringList *filteredKeys)
{
    if (!item)
        return QVariantMap();

    QVariantMap result = *item;
    result.insert(objectAttrAge, getInt(objectAttrAge, -1));

    if (filteredKeys)
    {
        for (int i=0;i<filteredKeys->count();++i)
            result.remove(filteredKeys->at(i));
    }

    return result;
}

bool TPAssociativeObject::contains(const QString key) const
{
    return item ? item->contains(key) : false;
}

void TPAssociativeObject::clearCachedValues()
{
    cachedAge = cachedLastPlayedAgo = -1;
}
