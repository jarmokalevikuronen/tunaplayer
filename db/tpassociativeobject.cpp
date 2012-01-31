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


TPAssociativeObject::TPAssociativeObject(const QString primaryKey)
{
    item = new TPAssociativeDBItem(primaryKey);
    itemOwned = true;
}


TPAssociativeObject::TPAssociativeObject(TPAssociativeDBItem *_item)
{
    itemOwned = false;
    item = _item;
}

TPAssociativeObject::~TPAssociativeObject()
{
    if (itemOwned)
        delete item;
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
    if (item)
    {
        QVariant ret = item->value(key);
        if (ret.isValid())
            return ret.toString();
    }
    return defaultValue;
}

bool TPAssociativeObject::save(int timeoutMs)
{
    if (item)
        return item->save(timeoutMs);
    return false;
}


