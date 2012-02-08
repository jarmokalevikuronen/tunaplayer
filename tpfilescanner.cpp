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
#include "tplog.h"

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
    maintainTimer = 0;

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
    maintainTimer = 0;

    qRegisterMetaType<TPFileScanner::State>("TPFileScanner::State");
}

TPFileScanner::~TPFileScanner()
{
    delete knownMediaFiles;
    delete newDbItems;
    delete maintainTimer;
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
    STATE() << "SCANNER: Maintain thread started";

    maintainCachedMediaFiles.clear();
    delete maintainTimer; maintainTimer = 0;

    // Returns true if/when asynchronous processing started
    // and mainloop needs to run.
    if (doMaintainCheck())
        QThread::exec();

    delete maintainTimer; maintainTimer = 0;

    STATE() << "SCANNER: Maintain thread finished";
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

void TPFileScanner::doMaintainCheckSlot()
{
    (void)doMaintainCheck();
}

bool TPFileScanner::doMaintainCheck()
{
    STATE() << "SCANNER: Maintain scan start";

    bool scanComplete = true;

    QStringList currentMediaFiles = mediaFiles();

    DEBUG() << "SCANNER: new files: " << currentMediaFiles.count();

    if (!equalItemsInList(currentMediaFiles, maintainCachedMediaFiles))
    {
        // Three minute interval for next check.
        maintainCachedMediaFiles = currentMediaFiles;
        scanComplete = false;
    }

    if (scanComplete)
    {
        DEBUG() << "SCANNER: Maintain scan complete. " << currentMediaFiles.count() << " new media files.";

        newDbItems = new QVector<TPAssociativeDBItem *>();

        for (int i=0;i<currentMediaFiles.count();++i)
        {
            TPAssociativeDBItem *item =
                    TPMediaFileMetaExtractor::extractTrackInfo(currentMediaFiles.at(i), maintainDb);
            if (item)
            {
                item->setValue(trackAttrFilename, currentMediaFiles.at(i));
                newDbItems->append(item);
            }
        }

        // This will notify the musicplayer core
        // that maintain scan is completed and results, if any, are
        // available.
        maintainCachedMediaFiles.clear();
        toState(MediaScannerMaintainScanComplete);
        QThread::exit(0);
    }
    else
    {
        DEBUG() << "SCANNER: maintain recheck scheduled for 3 minutes..";

        if (maintainTimer)
        {
            disconnect(maintainTimer, SIGNAL(timeout()), this, SLOT(doMaintainCheckSlot()));
            delete maintainTimer; maintainTimer = 0;
        }
        maintainTimer = new QTimer;
        connect(maintainTimer, SIGNAL(timeout()), this, SLOT(doMaintainCheckSlot()));

        maintainTimer->setSingleShot(true);
        maintainTimer->start(1000 *  60 * 3);
    }

    return scanComplete ? false : true;
}

bool TPFileScanner::equalItemsInList(QStringList &list1, QStringList &list2)
{
    if (list1.count() != list2.count())
        return false;

    // can not use direct list comparison as the
    // order of items in a list might be different.
    // It is not possible to have same item twice in a list as every
    // item represents a unique file in file system.
    // -> this kind of easy check is enough.
    for (int i=0;i<list1.count();++i)
    {
        if (!list2.contains(list1.at(i)))
            return false;
    }
    return true;
}

