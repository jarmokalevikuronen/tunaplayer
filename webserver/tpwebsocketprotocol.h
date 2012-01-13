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

#ifndef TPWEBSOCKETPROTOCOL_H
#define TPWEBSOCKETPROTOCOL_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QQueue>
#include "tpwebsocketprotocolmessage.h"
#include "tplibwebsocketinterface.h"

//! @class TPWebSocketProtocol
//! @brief Web socket low level protocol handling routines.
class TPWebSocketProtocol : public QObject
{
    Q_OBJECT

public:
    explicit TPWebSocketProtocol(TPWebSocketServer *_server, QObject *parent = 0);
    ~TPWebSocketProtocol();

signals:

    //! @brief emitted when ever a protocol message has been received.
    void protocolMessageReceived(TPWebSocketProtocol *protocol, TPWebSocketProtocolMessage message);

public slots:

    //! @brief handles a received protocol data unit by
    //! converting it to message instance.
    void handleDataReceived(const QByteArray data, void *origin);

    //! @brief Can be used to report events that will be delivered
    //! to all connected clients. Such events are for example state changes and alike.
    //!
    void sendEvent(TPWebSocketProtocolMessage event);

    //! @brief Responds to a specific command if any.
    void sendResponse(TPWebSocketProtocolMessage response);

private slots:

    void sendNow();

private:

    QTimer *sendTimer;

    // Queue for events that are to be delivered
    // to all the clients.
    QQueue<TPWebSocketProtocolMessage> pendingEvents;
    QQueue<TPWebSocketProtocolMessage> pendingResponses;

    //! Server instance.
    TPWebSocketServer *server;
};

#endif // TPWEBSOCKETPROTOCOL_H
