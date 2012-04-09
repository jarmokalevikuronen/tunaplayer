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

#ifndef TPASSOCIATIVEDBITEM_H
#define TPASSOCIATIVEDBITEM_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QDataStream>
#include <QFile>
#include <QVariant>
#include <QStringList>

// FORWARD DECLARATIONS
class TPAssociativeDB;

typedef QMap<QString, QVariant> TPMapModel;

//! @class TPAssociativeDBItem
//! @brief Represents a single object within a database. Object
//! is constructed from name-value pairs.
class TPAssociativeDBItem : public TPMapModel
{
    friend class TPAssociativeDB;

private:

    static int instanceCount;

public:

    explicit TPAssociativeDBItem(const QString primaryKeyValue, TPAssociativeDB *_db = 0);
    ~TPAssociativeDBItem();

    const QString primaryKey() const;

    void setValue(const QString key, const QVariant value);
    void setIntValue(const QString key, int value);
    void setStringValue(const QString key, const QString value);

    const QVariant value(const QString key) const;
    int intValue(const QString key, int defaultValue = 0) const;
    const QString stringValue(const QString key, const QString defaultValue="") const;

    void incIntValue(QString key, int by = 1);

    bool save(int timeoutMs);

private:

    //! Refers to DB this particular DB-ITEM object belongs to.
    TPAssociativeDB *db;
};

//! @class TPAssociativeDBItemVisitor
//! @brief Callback interface used when DB is enumerated.
class TPAssociativeDBItemVisitor
{
public:

    virtual void visitAssociativeDBItem(TPAssociativeDBItem *item) = 0;
};

#endif // TPASSOCIATIVEDBITEM_H


