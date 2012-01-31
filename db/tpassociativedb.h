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

#ifndef TPASSOCIATIVEDB_H
#define TPASSOCIATIVEDB_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QDataStream>
#include <QFile>
#include <QVariant>
#include <QStringList>
#include <QTimer>

#include "tpassociativedbitem.h"


typedef QMap<QString, TPAssociativeDBItem *> TPAssociativeDBModel;


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

    //! Flag indicating whether database is currently dirty and deservest to
    //! be saved to disk.
    bool dirty;
};


#endif // TPASSOCIATIVEDB_H

