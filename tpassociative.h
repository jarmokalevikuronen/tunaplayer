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

#ifndef TPASSOCIATIVE_H
#define TPASSOCIATIVE_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QDataStream>
#include <QFile>
#include <QVariant>
#include <QStringList>

typedef QMap<QString, QVariant> TPMapModel;

class TPAssociativeDB;
class QTimer;

//! @class TPAssociativeDBItem
//! @brief Represents a single object within a database. Object
//! is constructed from name-value pairs.
class TPAssociativeDBItem : public TPMapModel
{
public:

    explicit TPAssociativeDBItem(const QString primaryKeyValue, TPAssociativeDB *_db = 0);
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

    TPAssociativeDB *db;
};


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

    virtual QMap<QString, QVariant> toMap(QStringList *filteredKeys = NULL)
    {
        if (!item)
            return QVariantMap();

        if (filteredKeys)
        {
            QVariantMap result = *item;
            for (int i=0;i<(*filteredKeys).count();++i)
                result.remove((*filteredKeys).at(i));
            return result;
        }

        return *item;
    }

    void incIntValue(const QString key, int by = 1);

    bool save(int timeoutMs);

protected:

    bool itemOwned;
    TPAssociativeDBItem *item;
};


typedef QMap<QString, TPAssociativeDBItem *> TPAssociativeDBModel;

//! @class TPAssociativeDBItemVisitor
//! @brief Callback interface used when DB is enumerated.
class TPAssociativeDBItemVisitor
{
public:

    virtual void visitAssociativeDBItem(TPAssociativeDBItem *item) = 0;
};

//! @class TPAssociativeDB
//! @brief Class that implements a very simple database that is capable
//! to store associative objects (TPAssociativeDBItem).
class TPAssociativeDB : public QObject, public TPAssociativeDBModel
{
    Q_OBJECT

public:

    explicit TPAssociativeDB(QObject *parent, const QString _filename = "");
    ~TPAssociativeDB();

    TPAssociativeDBItem *item(const QString primaryKey);
    TPAssociativeDBItem* allocItem(const QString primaryKey);
    void removeItem(TPAssociativeDBItem *item);
    void addItem(TPAssociativeDBItem *item);
    void updateItem(TPAssociativeDBItem *item);
    bool save(int timeoutMs = 0);
    bool saveIfDirty(int timeoutMs = 0);
    bool saveIfDirty(TPAssociativeDBItem *object, int timeoutMs = 0);
    void visitItems(TPAssociativeDBItemVisitor *visitor);
    bool saveToTemp(TPAssociativeDBItem *object);
private:
    bool refreshFromTemp(TPAssociativeDBItem *object);

protected:

    bool open();

private slots:

    void executeSave();

private:

    QTimer *saveTimer;
    QString filename;
    // DB Identifier, derived from the filename.
    QString dbId;
    bool dirty;
};


#endif // TPASSOCIATIVE_H
