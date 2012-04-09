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
    // 1. import existing database into memory.
    //
    toState(TPFileScanner::MediaScannerUpgradeImportDB);
    dbs->load();

    //
    // 2. Scan all files in disk(s).
    //
    toState(TPFileScanner::MediaScannerUpgradeScanDisk);
    QStringList list = mediaFiles();

    //
    // 3. Build runtime DB structure
    //
    toState(TPFileScanner::MediaScannerUpgradeCheckNew);
    dbs->build(list);

    //
    // 4. Save changes to disk etc.
    //
    toState(TPFileScanner::MediaScannerUpgradeExportDB);
    processFinalTasks();
}

void TPFileScanner::doFullScan()
{
    if (!dbs)
        dbs = new TPDatabases;
    Q_ASSERT(dbs);

    qDebug() << "SCANNER: STATE: Start full scan";

    //
    // 1. List files in disk(s).
    //
    toState(TPFileScanner::MediaScannerFullScanDisk);
    QStringList list = mediaFiles();

    toState(TPFileScanner::MediaScannerFullProcessFiles);
    emit mediaScannerProgress(0, list.count());

    //
    // 2. Build runtime db structure in smaller fragments
    // and keep reporting status along the way.
    //
    int steps = list.count() / 50 + (list.count() % 50 ? 1 : 0);
    int processed = 0;

    for (int i=0;i<steps;++i)
    {
        QStringList fragment = list.mid(i * 50, 50);
        processed += fragment.length();
        dbs->build(fragment);
        emit mediaScannerProgress(processed, list.count());
    }
    list.clear();

    //
    // 3. Write data to disk.
    //
    toState(TPFileScanner::MediaScannerFullExportDB);
    processFinalTasks();
}

void TPFileScanner::processFinalTasks()
{
    dbs->buildFinished();
    dbs->moveToThread(parentThread);

    toState(TPFileScanner::MediaScannerComplete);

    emit mediaScannerComplete(dbs);
    dbs = NULL;
}


void TPFileScanner::listMediaFiles(QString folder, MediaType type, QStringList &list)
{
    QStringList nameFilters;

    // Should audio files be included?
    if (type == MediaTypeAudioFiles || type == MediaTypeAudioAndControlFiles)
        nameFilters << "*.mp3" << "*.ogg" << "*.MP3" << "*.OGG" << "*.wma" << "*.WMA";

    // Should control files be included?
    if (type == MediaTypeControlFiles || type == MediaTypeAudioAndControlFiles)
        nameFilters << "*.tt";

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
            // Add new if, and only if, existing one is not known!
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

QStringList TPFileScanner::mediaFiles(MediaType type)
{
    QStringList files;

    QStringList::iterator it = scanFolders.begin();
    while (it != scanFolders.end())
    {
        listMediaFiles(*it, type, files);
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

    //
    // 1. Scan DISK.
    //
    QStringList currentMediaFiles = mediaFiles(MediaTypeAudioFiles);

    DEBUG() << "SCANNER: new files: " << currentMediaFiles.count();

    //
    // 2. Check were there any changes?
    //
    if (!equalItemsInList(currentMediaFiles, maintainCachedMediaFiles))
    {
        // Three minute interval for next check.
        maintainCachedMediaFiles = currentMediaFiles;
        scanComplete = false;
    }

    if (scanComplete)
    {
        // TODO: How to deal with the user profile files
        // -> We definitely would like to create a new

        //
        // No changest to previous scan -> do report the status.
        //
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

        newUserTags = new QVector<TPUserTag *>();
        QStringList userTags = mediaFiles(MediaTypeControlFiles);
        foreach(QString tagfile, userTags)
        {
            QFileInfo fi(tagfile);
            TPUserTag *t = new TPUserTag(fi.absolutePath(), fi.fileName());
            if (t->isValid())
                newUserTags->append(t);
            else
            {
                ERROR() << "Invalid usertag file: " << tagfile;
                delete t;
            }
        }

        //
        // Try to reduce some memory consumption here
        //
        maintainCachedMediaFiles.clear();

        toState(MediaScannerMaintainScanComplete);

        QThread::exit(0);
    }
    else
    {
        //
        // Changes since previous scan -> delay for a while before doing the scan again.
        //
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


