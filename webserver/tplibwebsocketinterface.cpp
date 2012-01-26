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

#include <QFile>
#include "tplibwebsocketinterface.h"
#include "tpsettings.h"
#include "tpclargs.h"

static TPWebSocketServer *gWebSocketServer = 0;

struct libwebsocket_protocols TPWebSocketServer::protocols[] =
{
    {
        "http",
        TPWebSocketServer::callback_http,
        0,
        0,
        0,
        0,
        0
    }
    ,
    {
        "tp-json",
        TPWebSocketServer::callback_tp_json_protocol,
        0,
        0,
        0,
        0,
        0
    },
    {
        NULL,
        NULL,
        0,
        0,
        0,
        0,
        0
    }
};

TPWebSocketServer::TPWebSocketServer(QObject *parent) :
    QObject(parent)
{
    clientCount = 0;
    gWebSocketServer = this;

    port = TPCLArgs::instance().arg(TPCLArgs::cliArgHttpPort, 10088).toInt();
    if (port < 1 || port > 65535)
        port = 10087;

    context = libwebsocket_create_context(port, NULL, protocols,
                    libwebsocket_internal_extensions,
                    NULL, NULL, -1, -1, 0);

}

TPWebSocketServer::~TPWebSocketServer()
{
    if (context)
        libwebsocket_context_destroy(context);
}

int TPWebSocketServer::callback_tp_json_protocol(struct libwebsocket_context *context,
                        struct libwebsocket *wsi,
                        enum libwebsocket_callback_reasons reason,
                        void *user,
                        void *in,
                        size_t len)
{
    Q_UNUSED(context);
    Q_UNUSED(user);

//    qDebug() << "callback_tp_json_protocol: reason: " << reason;


    if (reason == LWS_CALLBACK_ESTABLISHED)
    {
        ++gWebSocketServer->clientCount;
        emit gWebSocketServer->clientConnected(wsi);
        qDebug() << "SOCKET: ESTABLISHED: " << wsi;
    }
    else if (reason == LWS_CALLBACK_CLOSED)
    {
        --gWebSocketServer->clientCount;
        emit gWebSocketServer->clientDisconnected(wsi);
        qDebug() << "SOCKET: CLOSED: " << wsi;
    }
    else if (reason == LWS_CALLBACK_RECEIVE)
    {
       QByteArray b((const char *)in, len);
       qDebug() << "SOCKET: RECEIVED: " << b.left(92);
       emit gWebSocketServer->messageReceived(b, wsi);
    }

    return 0;
}

int TPWebSocketServer::callback_http(struct libwebsocket_context *context,
                        struct libwebsocket *wsi,
                        enum libwebsocket_callback_reasons reason,
                        void *user,
                        void *in,
                        size_t len)
{
    Q_UNUSED(context);
    Q_UNUSED(len);
    int fd;


    switch (reason)
    {
    case LWS_CALLBACK_CLIENT_WRITEABLE:
//        qDebug() << "LWS_CALLBACK_CLIENT_WRITEABLE";
        break;

    case LWS_CALLBACK_SERVER_WRITEABLE:
  //      qDebug() << "LWS_CALLBACK_SERVER_WRITEABLE";
        break;

    case LWS_CALLBACK_HTTP:
    {
        qDebug() << "HTTP: SERVE: " << (char *)in;

        const char *http_request_path = (const char *)in;

        QString filePath;
        bool found = false;
        for (int i=0;!found && i<gWebSocketServer->providers.count();++i)
        {
            filePath = gWebSocketServer->providers.at(i)->objectToFile(http_request_path);
            if (QFile::exists(filePath))
                found = true;
        }

        QString contentType = gWebSocketServer->filenameToMime(filePath);
        DEBUG() << "HTTP: SERVE: file=" << filePath << " content:" << contentType;
        if (libwebsockets_serve_http_file(wsi, filePath.toUtf8().constData(), contentType.toUtf8().constData()))
        {
            ERROR() << "HTTP: SERVE";
        }
    }
    break;

    case LWS_CALLBACK_ADD_POLL_FD:
    {
        fd = (int)(long)user;
        TPWebSocketServerNotifier *notifier = new TPWebSocketServerNotifier(fd, wsi, gWebSocketServer);
        notifier->setEnabled(true);
        gWebSocketServer->notifiers.insert(fd, notifier);
        connect(notifier, SIGNAL(activated(int)), gWebSocketServer, SLOT(notifierActivated(int)));
    }
    break;

    case LWS_CALLBACK_DEL_POLL_FD:
    {
        fd = (int)(long)user;
        TPWebSocketServerNotifier *notifier = gWebSocketServer->notifierForFd(fd);
        if (notifier)
        {
            notifier->setEnabled(false);
            notifier->deleteLater();
            gWebSocketServer->notifiers.remove(fd);
        }
    }
    break;

    default:
        break;
    }

    return 0;
}

void TPWebSocketServer::notifierActivated(int fd)
{
    //qDebug() << "notifiedActivated: fd: " << fd;

    TPWebSocketServerNotifier* notifier = notifierForFd(fd);
    if (notifier)
    {
        notifier->setEnabled(false);
        libwebsocket_service(gWebSocketServer->context, 0);
        notifier->setEnabled(true);
    }
}

void TPWebSocketServer::sendMessageToAll(const QByteArray content)
{
    sendMessage(content, NULL, NULL);
}

void TPWebSocketServer::sendMessage(const QByteArray content, void *target, void *except)
{
    char *buffer = new char [content.length() + LWS_SEND_BUFFER_PRE_PADDING + LWS_SEND_BUFFER_POST_PADDING];
    if (!buffer)
        return; // false maybe.

    memcpy(&buffer[LWS_SEND_BUFFER_PRE_PADDING], content.constData(), content.length());

    QMap<int, TPWebSocketServerNotifier *>::iterator it = notifiers.begin();
    while (it != notifiers.end())
    {
        TPWebSocketServerNotifier *notifier = it.value();
        if ((!target || target == notifier->getWsi()) && notifier->getWsi() != except)
            libwebsocket_write(notifier->getWsi(), (unsigned char *) &buffer[LWS_SEND_BUFFER_PRE_PADDING], content.length(), LWS_WRITE_TEXT);
        ++it;
    }

    delete [] buffer;
}

int TPWebSocketServer::countClients()
{
    return clientCount;
}

const QString TPWebSocketServer::filenameToMime(const QString filename)
{
    QString lcfn = filename.toLower();

    if (lcfn.endsWith(".htm") || lcfn.endsWith(".html"))
        return QString("text/html");
    else if (lcfn.endsWith(".css"))
        return QString("text/css");
    else if (lcfn.endsWith(".bmp"))
        return QString("image/bmp");
    else if (lcfn.endsWith(".jpg") || lcfn.endsWith(".jpeg"))
        return QString("image/jpeg");
    else if (lcfn.endsWith(".png"))
        return QString("image/png");
    else if (lcfn.endsWith(".txt") || lcfn.endsWith(".plain"))
        return QString("text/plain");
    else if (lcfn.endsWith(".gif"))
        return QString("image/gif");
    else if (lcfn.endsWith(".js"))
        return QString("text/javascript");

    return QString("application/octet-stream");
}

