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

#ifndef TPLIBWEBSOCKETINTERFACE_H
#define TPLIBWEBSOCKETINTERFACE_H

#include <QDebug>

extern "C"
{
#include "3rdparty/libwebsocket/libwebsockets.h"
}


#include <QObject>
#include <QSocketNotifier>
#include <QMap>

#include "tpwebsocketdataproviderinterface.h"


class TPWebSocketServerNotifier : public QSocketNotifier
{
    Q_OBJECT

public:

    TPWebSocketServerNotifier(int fd, struct libwebsocket *_wsi, QObject *parent = 0) : QSocketNotifier(fd, QSocketNotifier::Read, parent)
    {
        wsi = _wsi;
    }

    ~TPWebSocketServerNotifier()
    {
        // TODO: Implement Me if needed..
    }

    inline struct libwebsocket *getWsi() const
    {
        return wsi;
    }

private:

    struct libwebsocket *wsi;
};

class TPWebSocketServer : public QObject
{
    Q_OBJECT

public:

    TPWebSocketServer(TPWebSocketDataProviderInterface *dataProvider, QObject *parent = 0);
    ~TPWebSocketServer();

    TPWebSocketServerNotifier* notifierForFd(int fd)
    {
        QMap<int, TPWebSocketServerNotifier *>::iterator it = notifiers.find(fd);
        if (it == notifiers.end())
            return NULL;

        TPWebSocketServerNotifier *notifier = it.value();
        return notifier;
    }

    //! Sends specific information to all connected clients
    //! connected using the tp-json protocol
    void sendMessageToAll(const QByteArray content);

    //! Sends a message to one or more connected clients.
    void sendMessage(const QByteArray dataToSend, void *target, void *except);

    //! Returns the amount of client connections currently present.
    int countClients();

private slots:

    void notifierActivated(int fd);

signals:

    //! Emit when message has been received from a source.
    void messageReceived(const QByteArray content, void *source);

    //! Emit if/when client gets disconnected.
    void clientDisconnected(void *source);

    //! Emit if/when client gets connected
    void clientConnected(void *source);

private:

    static int callback_http(struct libwebsocket_context *context,
                            struct libwebsocket *wsi,
                            enum libwebsocket_callback_reasons reason,
                            void *user,
                            void *in,
                            size_t len);

    static int callback_tp_json_protocol(struct libwebsocket_context *context,
                            struct libwebsocket *wsi,
                            enum libwebsocket_callback_reasons reason,
                            void *user,
                            void *in,
                            size_t len);

    private:

    const QString filenameToMime(const QString filename);

public:

    static struct libwebsocket_protocols protocols[];

    int port;

    struct libwebsocket_context *context;

    QMap<int, TPWebSocketServerNotifier *> notifiers;
private:

    //! Amount of connected clients.
    int clientCount;

    //! This is used to map web request paths to physical
    //! files within a disk.
    TPWebSocketDataProviderInterface *provider;
};



#endif // TPLIBWEBSOCKETINTERFACE_H
