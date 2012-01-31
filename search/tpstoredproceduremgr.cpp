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

#include "tpstoredproceduremgr.h"
#include "tppathutils.h"
#include <QDirIterator>
#include "tplog.h"
#include <QFileInfo>

TPStoredProcedureMgr::TPStoredProcedureMgr()
{
    createStoredProcedures();
}

TPStoredProcedureMgr::~TPStoredProcedureMgr()
{
    QMap<QString, TPStoredProcedure *>::iterator it = procedures.begin();
    while (it != procedures.end())
    {
        delete it.value();
        ++it;
    }
}

void TPStoredProcedureMgr::createStoredProcedures()
{
    QStringList files;

    QStringList nameFilters; nameFilters << "*.tsp" << "*.TSP";

    QDirIterator *dirIterator = new QDirIterator(
                    TPPathUtils::getTspFolder(),
                    nameFilters,
                    QDir::Readable | QDir::Files,
                    QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

    while (dirIterator->hasNext())
    {
        dirIterator->next();
        QString filename(dirIterator->fileInfo().absoluteFilePath());
        files.append(filename);
    }
    delete dirIterator;


    for (int i=0;i<files.count();++i)
    {
        DEBUG() << "TSP: Processing file: " << files.at(i);

        TPStoredProcedure *tsp = new TPStoredProcedure;
        if (!tsp)
            continue;

        if (tsp->compile(files.at(i)))
        {
            QFileInfo fi(files.at(i));
            procedures.insert(fi.baseName(), tsp);
        }
        else
        {
            ERROR() << "TSP: Failed to construct from " << files.at(i);
            ERROR() << "TSP: ErrorDescription: " << tsp->getErrorDescription();
            delete tsp;
        }
    }
}

TPStoredProcedure* TPStoredProcedureMgr::get(const QString name)
{
    QMap<QString, TPStoredProcedure *>::iterator it = procedures.find(name);
    if (it == procedures.end())
    {
        QString lcName = name.toLower();
        QMap<QString, TPStoredProcedure *>::iterator it = procedures.begin();
        while (it != procedures.end())
        {
            // Try also case insensitive matching in
            // case there would be typos..
            QString key = it.key();

            if (key.toLower() == lcName)
                return it.value();

            ++it;
        }
        return NULL;
    }

    return it.value();
}

