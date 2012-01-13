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

#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QTimer>
#include "musicplayercore.h"
#include <QCoreApplication>
#include "tppathutils.h"
#include "tplibwebsocketinterface.h"
#include "tpwebsocketvirtualfolder.h"
#include "tpwebsocketprotocol.h"
#include "tpsettings.h"
#include "tpclargs.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Initialize command line argument parser
    TPCLArgs::initialize(a.arguments());

    // Initialize more permanent settings.
    TPSettings::initialize(TPPathUtils::getSettingsFilename());

    // Create Web Server and a virtual directory handler/mapper
    TPWebSocketVirtualFolder *virtualFolder =
            new TPWebSocketVirtualFolder(TPPathUtils::getWebServerRootFolder(), "topsecret");
    TPWebSocketServer *server =
            new TPWebSocketServer(virtualFolder);
    TPWebSocketProtocol *protocol =
            new TPWebSocketProtocol(server);

    TPMusicPlayerCore *player = new TPMusicPlayerCore(protocol);

    player->start();

    int ret = a.exec();

    delete player;
    delete protocol;
    delete server;
    delete virtualFolder;

    TPSettings::release();
    TPCLArgs::release();

    return ret;
}
