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

#include <QCoreApplication>
#include <QtGlobal>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QDir>
#include "musicplayercore.h"
#include "tppathutils.h"
#include "tplibwebsocketinterface.h"
#include "tpwebsocketvirtualfolder.h"
#include "tpwebsocketprotocol.h"
#include "tpsettings.h"
#include "tpclargs.h"
#include "tpsignalhandler.h"
#include "tplog.h"

static int lock_fd = -1;

static bool takeLock()
{
    lock_fd = open("/tmp/tunaplayer.lock", O_CREAT|O_RDWR, 0666);
    if (lock_fd < 0)
        return false;

    if (lockf(lock_fd, F_TLOCK, 0) != 0)
        return false;

    return true;
}

static void releaseLock()
{
    if (lock_fd >= 0)
        close(lock_fd);
    lock_fd = -1;
}

static void copyAllFiles(const QString fromFolder, const QString toFolder)
{
    DEBUG() << "INIT: " << "ProcessFolder: " << fromFolder;
    QDir source(fromFolder);
    QDir target(toFolder);

    QStringList sourceFiles = source.entryList(QDir::Files);
    QString sourceFile;
    foreach(sourceFile, sourceFiles)
    {
        QFileInfo sourceFi(QString("%1%2").arg(fromFolder + QDir::separator()).arg(sourceFile));
        QFileInfo targetFi(QString("%1%2").arg(toFolder + QDir::separator()).arg(sourceFile));

        //
        // Check do we need to copy ..
        //
        if (!sourceFi.isSymLink() && !sourceFi.isDir() && sourceFi.isFile())
        {
            if (!targetFi.exists())
            {
                bool ok = QFile::copy(sourceFi.absoluteFilePath(), targetFi.absoluteFilePath());
                if (!ok)
                    ERROR() << "INIT: Failed Copying " << sourceFi.absoluteFilePath() << " to " << targetFi.absoluteFilePath();
                else
                    DEBUG() << "INIT: Copied " << sourceFi.absoluteFilePath() << " to " << targetFi.absoluteFilePath();


            }
        }
    }
}

static void copyDefaultsToHome()
{
    copyAllFiles(TPPathUtils::getPlaylistFolderRo(), TPPathUtils::getPlaylistFolder());
    copyAllFiles(TPPathUtils::getPlaylistArtFolderRo(), TPPathUtils::getPlaylistArtFolder());
    copyAllFiles(TPPathUtils::getWebServerRootFolderRo(), TPPathUtils::getWebServerRootFolder());
}

int main(int argc, char *argv[])
{
    if (!takeLock())
    {
        fprintf(stderr, "Only one instance can be run at a time\n");
        fflush(stderr);
        return 1;
    }

    QCoreApplication a(argc, argv);

    qInstallMsgHandler(tpMessageOutput);

    // Initialize command line argument parser
    if (TPCLArgs::initialize(a.arguments()).usageRequested())
    {
        fprintf(stdout, "%s\n\n", TPCLArgs::instance().getUsageText().toUtf8().constData());
        fflush(stdout);
        return 2;
    }

    // Init debug logging functionality.
    initDebugLogging();

    // Copy default playlists etc. from the
    // /usr/share/tunaplayer/* to the $HOME/.tunaplayer/*
    // copying only if not done so already
    copyDefaultsToHome();

    // Initialize more permanent settings.
    TPSettings::initialize(TPPathUtils::getSettingsFilename());

    QString ipFilter;
    ipFilter = TPCLArgs::instance().arg(TPCLArgs::cliArgIpFilter, QString("")).toString();
    if (!ipFilter.length())
        ipFilter = TPSettings::instance().get(settingIpFilter, QString("")).toString();

    // Create Web Server and a virtual directory handler/mapper
    TPWebSocketVirtualFolder *virtualFolderAccount =
            new TPWebSocketVirtualFolder(TPPathUtils::getWebServerRootFolder(),
                                         TPCLArgs::instance().arg(TPCLArgs::cliArgSecret, "topsecret").toString());
    TPWebSocketVirtualFolder *virtualFolderGlobal =
            new TPWebSocketVirtualFolder(TPPathUtils::getWebServerRootFolderRo(),
                                         TPCLArgs::instance().arg(TPCLArgs::cliArgSecret, "topsecret").toString());
    TPWebSocketServer *server = new TPWebSocketServer();
    server->addVirtualFolder(virtualFolderGlobal);
    server->addVirtualFolder(virtualFolderAccount);
    server->addIpFilter(ipFilter);

    TPWebSocketProtocol *protocol =
            new TPWebSocketProtocol(server);

    TPMusicPlayerCore *player = new TPMusicPlayerCore(protocol);

    TPSignalHandler *sh = new TPSignalHandler(0);

    QObject::connect(sh, SIGNAL(quit()), &a, SLOT(quit()));

    player->start();

    int ret = a.exec();

    delete sh;
    delete player;
    delete protocol;
    delete server;
    delete virtualFolderGlobal;
    delete virtualFolderAccount;

    TPSettings::release();
    TPCLArgs::release();

    releaseLock();

    return ret;
}
