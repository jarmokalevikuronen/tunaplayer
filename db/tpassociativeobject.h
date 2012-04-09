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

#ifndef TPASSOCIATIVEOBJECT_H
#define TPASSOCIATIVEOBJECT_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QDataStream>
#include <QFile>
#include <QVariant>
#include <QStringList>

#include "tpassociative.h"
#include "tpassociativemeta.h"

typedef QMap<QString, QVariant> TPMapModel;


//! @class TPAssociativeObject
//! @brief Higher level class that encapsulates one TPAssociativeDBItem object.
class TPAssociativeObject
{
public:

    explicit TPAssociativeObject(TPAssociativeDBItem *_item);
    explicit TPAssociativeObject(const QString primaryKey = "<No-Primary-Key>");
    ~TPAssociativeObject();

    void set(const QString key, QVariant value);
    void setInt(const QString key, int value);
    void setString(const QString key, const QString value);
    void setByteArray(const QString key, const QByteArray value);

    virtual const QVariant get(const QString key) const;
    virtual int getInt(const QString key, int defaultValue = 0) const;
    virtual const QString getString(const QString key, const QString defaultValue="") const;

    virtual void clearCachedValues();

    virtual QMap<QString, QVariant> toMap(QStringList *filteredKeys = NULL);

    void incIntValue(const QString key, int by = 1);

    bool save(int timeoutMs);

    //! @brief Checks whether given key is known
    bool contains(const QString key) const;

    //! @brief Detaches the item from this object. returns 0 in case dbitem is not
    //! owned and thus detaching is not feasible..
    TPAssociativeDBItem *detachDbItem();

private: // New impmentation

    inline bool isDynamic(const QString &key) const
    {
        return key.startsWith(dynamicTokenPrefix);
    }

protected:

    //! Dynamic properties not in scope of permanent saving.
    QMap<QString, QVariant> dynamicProperties;
    //! Mutable as cache updated in const method
    mutable int cachedAge;
    //! Mutable as cache updated in const method.
    mutable int cachedLastPlayedAgo;
    //! Do we own the /item/ variable and can delete it when quitting.
    bool itemOwned;
    //! DB Object this higher level object represents.
    TPAssociativeDBItem *item;
};

#endif // TPASSOCIATIVEOBJECT_H
