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

#ifndef TPFILESCANNER_H
#define TPFILESCANNER_H

#include <QString>
#include <QDirIterator>
#include <QThread>
#include <QTimer>
#include <QStringList>
#include <QEventLoop>
#include "db/tpdatabases.h"
#include "tpassociative.h"
#include "tpusermanager.h"

class TPAlbum;

//! @class TPFileScanner
//! @brief Object that runs in its own thread and is responsible
//! of scanning the directories for music content. Basically operates
//! in three different modes
//!  INITIAL: When started and scanning for the first time (=no DB constructed just yet)
//!  UPGRADE: When started but DB exists -> DB content is updated
//!  MAINTAIN: When performing periodical rescans at runtime -> will just report a list
//!  of new files (note: current expectation is that there will be no files removed from the Jukebox system).
//! NOTE: Also scans for user access token files (*.tt) that specify which users/profiles like to
//! see the content in that folder, or its subfolders.
//!
class TPFileScanner : public QThread
{
    Q_OBJECT

public:

    //! C++ constructor for INITIAL and UPGRADE scans.
    TPFileScanner(TPDatabases *_db, QStringList scanFolders, QString dbFile);

    //! C++ constructor for MAINTAIN scans.
    TPFileScanner(const QStringList &scanFolders, QStringList *existingFiles, TPAssociativeDB *_maintainDb);

    //! C++ destructor
    ~TPFileScanner();

    enum State
    {
        MediaScannerUnknown,
        MediaScannerUpgradeImportDB,
        MediaScannerUpgradeScanDisk,
        MediaScannerUpgradeVerifyExisting,
        MediaScannerUpgradeCheckNew,
        MediaScannerUpgradeExportDB,
        MediaScannerFullScanDisk,
        MediaScannerFullProcessFiles,
        MediaScannerFullExportDB,
        MediaScannerComplete,
        MediaScannerMaintainScanComplete
    };

    inline State getState() const
    {
        return state;
    }

    //! @brief Gets the results of the maintenance run
    //! @return array of pointers to DB items. Owhership
    //! of the array, and its content, transfers to caller.
    //! NULL is returned if called multiple times.
    inline QVector<TPAssociativeDBItem *> * maintainResults()
    {
        QVector<TPAssociativeDBItem *> *ret = newDbItems;
        newDbItems = 0;
        return ret;
    }

private:

    void doFullScan();
    void doUpgradeScan();
    void doMaintainScan();
    void listMediaFiles(QString folder, QStringList &list);
    void run();

    void toState(State _state);

    void processFinalTasks();

    QStringList mediaFiles();

    bool equalItemsInList(QStringList &list1, QStringList &list2);

    bool doMaintainCheck();



private slots:

    void doMaintainCheckSlot();

signals:

    void mediaScannerProgress(int currentEntry, int totalEntries);
    void mediaScannerStateChanged(TPFileScanner::State state);
    void mediaScannerComplete(TPDatabases *db);

private:

    //! Current state of operation. volatiled as the state is read also from
    //! another thread (=main thread).
    volatile enum State state;

    //! Folders to scan
    QStringList scanFolders;

    //! DB File for results.
    QString dbFile;

    //! Parent thread where all the constructed
    //! qobject derived classes are moved once
    //! finished.
    QThread *parentThread;

    //! Database(s) under construction..
    TPDatabases *dbs;

    //! Used during maintain scan as a reference
    //! of existing media against the new media files
    //! list is to be compared.
    QStringList *knownMediaFiles;

    //! New associative db items constructed
    //! during maintain scan and handed over
    //! once maintain is complete.
    QVector<TPAssociativeDBItem *> *newDbItems;

    //! Just a pointer to TrackDB that is used
    //! to initialize new TPAssociativeDBItem objects
    //! created during maintain scan. Not really used
    //! within this maintain thread.
    TPAssociativeDB *maintainDb;

    //! In case maintain scan recognizes changes in content,
    //! It will launch a timer for rescan until two subsequent
    //! scans produce equal result set. This is to overcome issues
    //! where meta extractor gets false results when trying extract
    //! information from albums/files when they are being copied.
    QTimer *maintainTimer;

    //! Cached media files used to track changes.
    QStringList maintainCachedMediaFiles;
};

#endif // FILESCANNER_H
