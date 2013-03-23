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

#include "tpwebsocketprotocol.h"
#include <QDebug>

TPWebSocketProtocol::TPWebSocketProtocol(TPWebSocketServer *_server, QObject *parent) :
    QObject(parent)
{
    server = _server;
    Q_ASSERT(server);

    connect(server, SIGNAL(messageReceived(QByteArray,void*)), this, SLOT(handleDataReceived(QByteArray, void*)));
    connect(server, SIGNAL(clientDisconnected(int,void*)), this, SLOT(handleClientDisconnected(int,void *)));
}

TPWebSocketProtocol::~TPWebSocketProtocol()
{
    disconnect();
}

void TPWebSocketProtocol::handleDataReceived(const QByteArray data, void *origin)
{
    TPWebSocketProtocolMessage msg;
    msg.initializeFromSerialized(data);
    msg.setOrigin(origin);

    DEBUG() << "PROTOCOL: RECEIVED: " << data;

    if (msg.isOk())
        emit protocolMessageReceived(this, msg);
}

void TPWebSocketProtocol::handleClientDisconnected(int clientCount, void *clientHandle)
{
    emit protocolClientDisconnected(this, clientCount, clientHandle);
}

void TPWebSocketProtocol::sendEvent(TPWebSocketProtocolMessage event)
{
    Q_ASSERT(event.eventFilter() != 0);

    if (server->countClients())
    {
        pendingEvents.append(event);

        sendAll();
    }
}

void TPWebSocketProtocol::sendResponse(TPWebSocketProtocolMessage response)
{
    // Client disconnected in the meanwhile -> just discard the response.
    if (server->countClients())
    {
        if (response.getOrigin())
        {
            pendingResponses.append(response);

            sendAll();
        }
    }
}

void TPWebSocketProtocol::sendAll()
{
    while (sendNow());
}

bool TPWebSocketProtocol::sendNow()
{
    // Firstly, always process events to keep UI as up to date as possible.
    if (pendingEvents.count())
    {
        server->sendFilteredEvent(pendingEvents.takeFirst());

        return true;
    }
    else if (pendingResponses.count())
    {
        server->sendMessage(pendingResponses.takeFirst());

        return true;
    }

    return false;
}


