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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "tplibwebsocketinterface.h"
#include "tpsettings.h"
#include "tpclargs.h"
#include "tplog.h"

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
    filter = 0;

    clientCount = 0;
    gWebSocketServer = this;

    port = TPCLArgs::instance().arg(TPCLArgs::cliArgHttpPort, 10088).toInt();
    if (port < 1 || port > 65535)
        port = 10087;

    context = libwebsocket_create_context(port, NULL, protocols,
                    libwebsocket_internal_extensions,
                    NULL, NULL, -1, -1, 0);

    Q_ASSERT(context);

    DEBUG() << "HTTP Serving at port: " << port;
}

TPWebSocketServer::~TPWebSocketServer()
{
    if (context)
        libwebsocket_context_destroy(context);
    delete filter;
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


    if (reason == LWS_CALLBACK_ESTABLISHED)
    {
        ++gWebSocketServer->clientCount;
        emit gWebSocketServer->clientConnected(wsi);
        DEBUG() << "WEBSOCKET: Connected. Total " << gWebSocketServer->clientCount << " clients";
    }
    else if (reason == LWS_CALLBACK_CLOSED)
    {
        --gWebSocketServer->clientCount;
        emit gWebSocketServer->clientDisconnected(wsi);
        DEBUG() << "WEBSOCKET: Disconnected. Total " << gWebSocketServer->clientCount << " clients";
    }
    else if (reason == LWS_CALLBACK_RECEIVE)
    {
       QByteArray b((const char *)in, len);
       emit gWebSocketServer->messageReceived(b, wsi);
    }
    else if (reason == LWS_CALLBACK_SERVER_WRITEABLE)
    {
        libwebsocket_callback_on_writable(context, wsi);
        DEBUG() << "WEBSOCKET: ServerWriteable: " << wsi;
    }
/*    else if (LWS_EXT_CALLBACK_REQUEST_ON_WRITEABLE == reason)
    {
        DEBUG() << "WEBSOCKET: RequestOnWritable";
        return 1;
    }
    else if (LWS_EXT_CALLBACK_IS_WRITEABLE == reason)
    {
        DEBUG() << "WEBSOCKET: CallbackIsWritable";
        return 1;
    }*/

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
    int fd = (int)(long)user;

    switch (reason)
    {
        case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
        {
            // If no filter configured -> allow any IP.
            if (!gWebSocketServer->filter)
                return 0;

            char ipstr[64];
            struct sockaddr_storage ss;
            len = sizeof ss;
            int status = getpeername(fd, (struct sockaddr *)&ss, &len);
            if (status < 0)
            {
                ERROR() << "HTTP: SERVE: getpeername failed (" << errno << ") for socket #" << fd;
                return 1; // Will not be accpted.
            }

            // deal with both IPv4 and IPv6:
            if (ss.ss_family == AF_INET)
            {
                struct sockaddr_in *s = (struct sockaddr_in *)&ss;
                inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
            }
            else
            { // AF_INET6
                struct sockaddr_in6 *s = (struct sockaddr_in6 *)&ss;
                inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
            }            

            bool passed = gWebSocketServer->filter->match(QByteArray(ipstr));

            if (passed)
                DEBUG() << "Accepting connection from: " << ipstr;
            else
                DEBUG() << "Denying connection from: " << ipstr;

            return passed ? 0 : 1;

        }
        break;

        case LWS_CALLBACK_HTTP:
        {
            DEBUG() << "HTTP: SERVE: " << (char *)in;

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
        DEBUG() << "HTTP: AddFD: " << fd;

        TPWebSocketServerNotifier *notifier = new TPWebSocketServerNotifier(fd, wsi, gWebSocketServer);
        notifier->setEnabled(true);
        gWebSocketServer->notifiers.insert(fd, notifier);
        connect(notifier, SIGNAL(activated(int)), gWebSocketServer, SLOT(notifierActivated(int)));
    }
    break;

    case LWS_CALLBACK_DEL_POLL_FD:
    {
        DEBUG() << "HTTP: RemoveFD: " << fd;
        TPWebSocketServerNotifier *notifier = gWebSocketServer->notifierForFd(fd);
        if (notifier)
        {
            gWebSocketServer->removeNotifier(fd);
            delete notifier;
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
    TPWebSocketServerNotifier* notifier = notifierForFd(fd);
    if (notifier)
    {
        notifier->setEnabled(false);
        libwebsocket_service(gWebSocketServer->context, 0);
        // Above call might cause the notifier to be deleted ->
        // refetch the pointer here.
        notifier = notifierForFd(fd);
        if (notifier)
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

