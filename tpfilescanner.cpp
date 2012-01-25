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

#include <QDir>
#include <QFileInfoList>
#include <QDebug>
#include <QStringList>
#include <QMetaType>
#include <QThread>
#include <QTime>
#include "tpfilescanner.h"
#include "tptrack.h"
#include "tpalbum.h"
#include "db/tpdatabases.h"
#include "db/tptrackdb.h"
#include "db/tpartistdb.h"
#include "db/tpalbumdb.h"
#include "albumartutil.h"
#include "tpmediafilemetaextractor.h"

Q_DECLARE_METATYPE(TPFileScanner::State);

TPFileScanner::TPFileScanner(TPDatabases *_db, QStringList folders, QString _dbFile)
{
    dbs = _db;
    dbFile = _dbFile;
    scanFolders = folders;
    state = MediaScannerUnknown;
    parentThread = QThread::currentThread();
    knownMediaFiles = 0;
    newDbItems = 0;
    maintainDb = 0;

    qRegisterMetaType<TPFileScanner::State>("TPFileScanner::State");
}

TPFileScanner::TPFileScanner(const QStringList &_scanFolders, QStringList *existingFiles, TPAssociativeDB *_maintainDb)
{
    scanFolders = _scanFolders;
    knownMediaFiles = existingFiles;
    parentThread = QThread::currentThread();
    state = MediaScannerUnknown;
    newDbItems = 0;
    maintainDb = _maintainDb;

    qRegisterMetaType<TPFileScanner::State>("TPFileScanner::State");
}

TPFileScanner::~TPFileScanner()
{
    delete knownMediaFiles;
    delete newDbItems;
}

void TPFileScanner::run()
{
    setPriority(QThread::IdlePriority);

    if (knownMediaFiles)
        doMaintainScan();
    else
    {
        QFile f(dbFile);
        if (f.exists(dbFile))
            doUpgradeScan();
        else
            doFullScan();
    }
}

void TPFileScanner::listMediaFiles(QString folder, QStringList &list)
{
    // TODO: Add more file extensions here (configurable?)
    QStringList nameFilters; nameFilters << "*.mp3" << "*.ogg" << "*.MP3" << "*.OGG" << "*.wma" << "*.WMA";

    QDirIterator *dirIterator = new QDirIterator(
                    folder,
                    nameFilters,
                    QDir::Readable | QDir::Files,
                    QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

    if (knownMediaFiles)
    {
        while (dirIterator->hasNext())
        {
            dirIterator->next();
            QString filename(dirIterator->fileInfo().absoluteFilePath());
            if (!knownMediaFiles->contains(filename))
                list.append(filename);
        }
    }
    else
    {
        while (dirIterator->hasNext())
        {
            dirIterator->next();
            QString filename(dirIterator->fileInfo().absoluteFilePath());
            list.append(filename);
        }
    }
    delete dirIterator;
}

void TPFileScanner::toState(State _state)
{
    if (_state != state)
        emit mediaScannerStateChanged(state = _state);
}

void TPFileScanner::doMaintainScan()
{
    qDebug() << "SCANNER: STATE: Maintain scan start";

    QStringList newFiles = mediaFiles();

    qDebug() << "SCANNER: STATE: Maintain found " << newFiles.count() << " new media files";

    if (newFiles.count())
    {
        newDbItems = new QVector<TPAssociativeDBItem *>();

        for (int i=0;i<newFiles.count();++i)
        {
            TPAssociativeDBItem *item =
                    TPMediaFileMetaExtractor::extractTrackInfo(newFiles.at(i), maintainDb);
            if (item)
            {
                item->setValue(trackAttrFilename, newFiles.at(i));
                newDbItems->append(item);
            }
        }
    }

    // This will notify the musicplayer core
    // that maintain scan is completed and results, if any, are
    // available.
    toState(MediaScannerMaintainScanComplete);
}

void TPFileScanner::doUpgradeScan()
{
    qDebug() << "SCANNER: STATE: Upgrade scan start";

    if (!dbs)
        dbs = new TPDatabases;
    Q_ASSERT(dbs);

    //
    // Firstly, import the database into memory.
    //
    toState(TPFileScanner::MediaScannerUpgradeImportDB);
    TPTrackDB *trackDB = dbs->getTrackDB();
    qDebug() << "SCANNER: STATE: Upgrade scan initial tracks " << trackDB->count();

    //
    // Finally, do process possible new items if any.
    //
    toState(TPFileScanner::MediaScannerUpgradeScanDisk);
    QStringList list = mediaFiles();

    toState(TPFileScanner::MediaScannerUpgradeCheckNew);
    trackDB->processFiles(list);

    // Always save to disk (for now at least).
    toState(TPFileScanner::MediaScannerUpgradeExportDB);

    processFinalTasks();
}

void TPFileScanner::doFullScan()
{
    if (!dbs)
        dbs = new TPDatabases;
    Q_ASSERT(dbs);

    qDebug() << "SCANNER: STATE: Start full scan";

    toState(TPFileScanner::MediaScannerFullScanDisk);
    QStringList list = mediaFiles();

    toState(TPFileScanner::MediaScannerFullProcessFiles);
    emit mediaScannerProgress(0, list.count());

    int steps = list.count() / 50 + (list.count() % 50 ? 1 : 0);
    int processed = 0;

    for (int i=0;i<steps;++i)
    {
        QStringList fragment = list.mid(i * 50, 50);
        processed += fragment.count();

        dbs->getTrackDB()->processFiles(fragment);
        emit mediaScannerProgress(processed, list.count());
    }
    list.clear();

    toState(TPFileScanner::MediaScannerFullExportDB);

    processFinalTasks();
}

void TPFileScanner::processFinalTasks()
{
    dbs->getTrackDB()->executePostCreateTasks();
    dbs->getTrackDB()->save();

    dbs->getAlbumDB()->executePostCreateTasks();
    dbs->getAlbumDB()->save();

    dbs->getArtistDB()->executePostCreateTasks();
    dbs->getArtistDB()->save();

    //
    // Feeds are not to be touched here.
    //
    dbs->moveToThread(parentThread);

    toState(TPFileScanner::MediaScannerComplete);

    emit mediaScannerComplete(dbs);
    dbs = NULL;
}

QStringList TPFileScanner::mediaFiles()
{
    QStringList files;

    QStringList::iterator it = scanFolders.begin();
    while (it != scanFolders.end())
    {
        listMediaFiles(*it, files);
        ++it;
    }

    return files;
}
