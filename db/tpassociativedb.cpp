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

#include "tpassociative.h"

TPAssociativeDB::TPAssociativeDB(QObject *parent, const QString _filename) : QObject(parent)
{
    dirty = false;
    saveTimer = NULL;
    filename = _filename;
    if (filename.length())
    {
        QFileInfo fi(filename);
        dbId = fi.baseName();
        open();
    }
    else
        dbId = "--nodbid--";
}

TPAssociativeDB::~TPAssociativeDB()
{
    TPAssociativeDBModel::iterator it = begin();
    while (it != end())
    {
        TPAssociativeDBItem *item = it.value();
        delete item;
        ++it;
    }
}

TPAssociativeDBItem* TPAssociativeDB::item(const QString primaryKey)
{
    TPAssociativeDBModel::const_iterator it = find(primaryKey);
    return it == end() ? NULL : it.value();
}

TPAssociativeDBItem* TPAssociativeDB::allocItem(const QString primaryKey)
{
    TPAssociativeDBItem *_item = item(primaryKey);
    if (_item)
        return _item;

    _item = new TPAssociativeDBItem(primaryKey, this);
    addItem(_item);

    return _item;
}

void TPAssociativeDB::removeItem(TPAssociativeDBItem *item)
{
    if (item)
    {
        remove(item->primaryKey());
        delete item;
        dirty = true;
    }
}

void TPAssociativeDB::addItem(TPAssociativeDBItem *item)
{
    Q_ASSERT(item);
    Q_ASSERT(item->primaryKey().length());

    insert(item->primaryKey(), item);
    dirty = true;
}

void TPAssociativeDB::updateItem(TPAssociativeDBItem *item)
{
    Q_UNUSED(item);
    dirty = true;
}

bool TPAssociativeDB::save(int timeoutMs)
{
    if (!filename.length())
        return false;

    if (timeoutMs > 0)
    {
        if (!saveTimer)
        {
            saveTimer = new QTimer(this);
            connect(saveTimer, SIGNAL(timeout()), this, SLOT(executeSave()));
        }

        saveTimer->stop();
        saveTimer->setSingleShot(true);
        saveTimer->start(timeoutMs);

        return true;
    }

    if (saveTimer)
        saveTimer->stop();

    QFile fout(filename + ".tmp");
    fout.remove();
    if (!fout.open(QIODevice::WriteOnly))
        return false;

    QDataStream dsOut(&fout);

    dsOut << size();
    // Go through all the items accordingly.
    TPAssociativeDBModel::const_iterator it = begin();
    while (it != end())
    {
        dsOut << it.key();
        TPMapModel sm = *it.value();
        dsOut << sm;
        ++it;
    }

    fdatasync(fout.handle());
    fout.close();

    QFile::remove(filename);
    dirty = false;

    return QFile::rename(filename + ".tmp", filename);
}

bool TPAssociativeDB::saveToTemp(TPAssociativeDBItem *object)
{
    Q_ASSERT(object);

    QString tempFilename = TPPathUtils::getDbItemTempFilename(dbId, object->primaryKey());

    QFile tempFile(tempFilename);
    // Remove possible existing item if any.
    tempFile.remove();
    if (!tempFile.open(QIODevice::WriteOnly))
        return false;

    QDataStream dsOut(&tempFile);
    TPMapModel *model = object;

    dsOut << *model;

    fdatasync(tempFile.handle());
    tempFile.close();

    return true;
}

bool TPAssociativeDB::refreshFromTemp(TPAssociativeDBItem *object)
{
    Q_ASSERT(object);

    QString tempFilename = TPPathUtils::getDbItemTempFilename(dbId, object->primaryKey());

    QFile tempFile(tempFilename);
    if (!tempFile.open(QIODevice::ReadOnly))
        return false;

    QDataStream dsIn(&tempFile);
    TPMapModel *model = (TPMapModel *)object;

    dsIn >> *model;

    tempFile.remove();
    tempFile.close();

    dirty = true;

    return true;
}

bool TPAssociativeDB::saveIfDirty(int timeoutMs)
{
    return dirty ? save(timeoutMs) : true;
}

bool TPAssociativeDB::open()
{
    QFile fin(filename);
    if (!fin.open(QIODevice::ReadOnly))
        return false;
    QDataStream dsIn(&fin);
    int items = 0;
    dsIn >> items;

    dirty = false;

    for (int i=0;i<items;++i)
    {
        QString key;
        dsIn >> key;
        TPAssociativeDBItem *item = new TPAssociativeDBItem(key, this);
        dsIn >> (*((TPMapModel *)item));
        refreshFromTemp(item);
        addItem(item);
    }

    fin.close();

    return true;
}

void TPAssociativeDB::executeSave()
{
    save(0);
}

void TPAssociativeDB::visitItems(TPAssociativeDBItemVisitor *visitor)
{
    Q_ASSERT(visitor);

    TPAssociativeDBModel::iterator it = begin();
    while (it != end())
    {
        TPAssociativeDBItem *item = it.value();
        visitor->visitAssociativeDBItem(item);
        ++it;
    }
}
