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

#include "tpassociativedbitem.h"
#include "tpassociativedb.h"

int TPAssociativeDBItem::instanceCount = 0;
static const QString __primaryKey("_pk_");

TPAssociativeDBItem::TPAssociativeDBItem(const QString primaryKeyValue, TPAssociativeDB *_db)
{
    ++instanceCount;

    db = _db;

    Q_ASSERT(primaryKeyValue.length() > 0);
    insert(__primaryKey, QVariant(primaryKeyValue));
}

TPAssociativeDBItem::~TPAssociativeDBItem()
{
    --instanceCount;

    if (!instanceCount)
        DEBUG() << "ASSOCIATIVE: " << " Last DBItem Deleted";
}

const QString TPAssociativeDBItem::primaryKey() const
{
    return value(__primaryKey).toString();
}

void TPAssociativeDBItem::setValue(const QString key, const QVariant _value)
{
    if (value(key) != _value)
    {
        insert(key, _value);
        if (db)
            db->updateItem(this);
    }
}

void TPAssociativeDBItem::setStringValue(const QString key, const QString _value)
{
    setValue(key, QVariant(_value));
}

void TPAssociativeDBItem::setIntValue(const QString key, int value)
{
    setValue(key, QVariant(value));
}

const QString TPAssociativeDBItem::stringValue(const QString key, const QString defaultValue) const
{
    QVariant val = value(key);
    if (val.isValid())
        return val.toString();
    return defaultValue;
}

const QVariant TPAssociativeDBItem::value(const QString key) const
{
    TPMapModel::const_iterator it = find(key);
    return it == end() ? QVariant() : it.value();
}

int TPAssociativeDBItem::intValue(const QString key, int defaultValue) const
{
    int retVal = defaultValue;
    bool ok = false;
    QVariant val = value(key);
    if (val.isValid())
        retVal = val.toInt(&ok);

    return ok ? retVal : defaultValue;
}

void TPAssociativeDBItem::incIntValue(QString key, int by)
{
    setIntValue(key, intValue(key, 0) + by);
}

bool TPAssociativeDBItem::save(int timeoutMs)
{
    Q_UNUSED(timeoutMs);

    if (db)
        return db->saveToTemp(this);

    return false;
}

