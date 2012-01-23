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

    sendTimer = new QTimer(this);
    sendTimer->setSingleShot(true);
    connect(sendTimer, SIGNAL(timeout()), this, SLOT(sendNow()));
}

TPWebSocketProtocol::~TPWebSocketProtocol()
{
    disconnect();
}

void TPWebSocketProtocol::handleDataReceived(const QByteArray data, void *origin)
{
    qDebug() << "WEBSOCKET: RECEIVED-LENGTH: " << data.length();

    TPWebSocketProtocolMessage msg;
    msg.initializeFromSerialized(data);
    msg.setOrigin(origin);

    if (msg.isOk())
        emit protocolMessageReceived(this, msg);
}

void TPWebSocketProtocol::sendEvent(TPWebSocketProtocolMessage event)
{
    if (server->countClients())
    {
        pendingEvents.append(event);
        if (!sendTimer->isActive())
            sendTimer->start(1);
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
            if (!sendTimer->isActive())
                sendTimer->start(1);
        }
    }
}

void TPWebSocketProtocol::sendNow()
{
    // Firstly, always process events to keep UI as up to date as possible.
    if (pendingEvents.count())
    {
        TPWebSocketProtocolMessage event = pendingEvents.takeFirst();
        server->sendMessageToAll(event.serialize());
    }
    else if (pendingResponses.count())
    {
        TPWebSocketProtocolMessage response = pendingResponses.takeFirst();
        server->sendMessage(response.serialize(), response.getOrigin(), 0);
    }

    // Start sending in deferred manner.
    if (pendingResponses.count() || pendingEvents.count())
        sendTimer->start(0);

//    qDebug() << "WEBSOCKET: SEND: QUEUE: Responses=" << pendingResponses.count() << " Events=" << pendingEvents.count();
}
