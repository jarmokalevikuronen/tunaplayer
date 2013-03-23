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

#include "tplog.h"
#include "tpwebsocketipaddressmask.h"
#include "tpwebsocketprotocolmessage.h"
#include <QVector>
#include <QTimer>

extern "C"
{
#include <libwebsockets.h>
}


#include <QObject>
#include <QSocketNotifier>
#include <QMap>

#include "tpwebsocketdataproviderinterface.h"


class TPWebSocketServerNotifier : public QSocketNotifier
{
    Q_OBJECT

public:

    TPWebSocketServerNotifier(int fd,
                              Type type,
                              struct libwebsocket *_wsi,
                              QObject *parent = 0) :
        QSocketNotifier(fd, type, parent)
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

//! @class TPWebSocketServer
//! @brief HTTP + WebSocket server. Mainly responsible of adapting
//! the functionality provided by the libwebsocet into Qt mainloop.
//! Also deals with some ip address filtering and such tasks.
class TPWebSocketServer : public QObject
{
    Q_OBJECT

public:

    TPWebSocketServer(QObject *parent = 0);
    ~TPWebSocketServer();

    void addVirtualFolder(TPWebSocketDataProviderInterface *dataProvider)
    {
        providers.append(dataProvider);
    }

    inline void addIpFilter(const QString ipFilter)
    {
        delete filter; filter = 0;
        filter = new TPWebSocketIPAddressMask(ipFilter);
    }

    TPWebSocketServerNotifier* readNotifierForFd(int fd);
    TPWebSocketServerNotifier* writeNotifierForFd(int fd);

    void removeNotifier(int fd)
    {
        QMap<int, TPWebSocketServerNotifier *>::iterator it;

        it = notifiersRead.find(fd);
        if (it != notifiersRead.end())
            delete it.value();
        it = notifiersWrite.find(fd);
        if (it != notifiersWrite.end())
            delete it.value();

        notifiersRead.remove(fd);
        notifiersWrite.remove(fd);
    }

    //! Sends a event to one or more recipients. In practise event is delivered
    //! to all connected clients, but certain events can be filtered (filter passed
    //! as member variable of the message).
    void sendFilteredEvent(TPWebSocketProtocolMessage message);

    //! Sends a message to one client (response to a command). Target
    //! is specified by the message.getOrigin().
    void sendMessage(TPWebSocketProtocolMessage message);

    //! Returns the amount of client connections currently present.
    int countClients();

private slots:

    void notifierActivated(int fd);

  //  void timedServe();

signals:

    //! Emit when message has been received from a source.
    void messageReceived(const QByteArray content, void *source);

    //! Emit if/when client gets disconnected.
    void clientDisconnected(int countConnected, void *source);

    //! Emit if/when client gets connected
    void clientConnected(int countConnected, void *source);

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

private:

    static struct libwebsocket_protocols protocols[];

    int port;

    struct libwebsocket_context *context;

    QMap<int, TPWebSocketServerNotifier *> notifiersRead;
    QMap<int ,TPWebSocketServerNotifier *> notifiersWrite;

private:

    //! IP Address filtering utility used to check the connected
    //! peer(s).
    TPWebSocketIPAddressMask *filter;

    //! Amount of connected clients.
    int clientCount;

    //! This is used to map web request paths to physical
    //! files within a disk.
    QVector<TPWebSocketDataProviderInterface *> providers;

//    QTimer serveTimer;
};



#endif // TPLIBWEBSOCKETINTERFACE_H
