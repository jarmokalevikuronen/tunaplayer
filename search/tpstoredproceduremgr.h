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

#ifndef TPSTOREDPROCEDUREMGR_H
#define TPSTOREDPROCEDUREMGR_H

#include <QObject>
#include <QMap>

#include "tpstoredprocedure.h"

class TPStoredProcedureMgr : public QObject
{
    Q_OBJECT

public:

    explicit TPStoredProcedureMgr(QObject *parent = 0);
    ~TPStoredProcedureMgr();

    TPStoredProcedure* get(const QString name);

signals:

public slots:

private: // Implementation

    //! Scans a disk and
    void createStoredProcedures();

private: // Data

    QMap<QString, TPStoredProcedure *> procedures;
};

#endif // TPSTOREDPROCEDUREMGR_H
