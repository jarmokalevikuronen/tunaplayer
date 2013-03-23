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
	0
    }
    ,
    {
        "tp-json",
        TPWebSocketServer::callback_tp_json_protocol,
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
	0
    }
};

TPWebSocketServer::TPWebSocketServer(QObject *parent) :
    QObject(parent)
{
    /*
    connect(&serveTimer,
            SIGNAL(timeout()),
            this,
            SLOT(timedServe()));
    serveTimer.setInterval(10);
    serveTimer.setSingleShot(false);
    serveTimer.start();
*/
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.gid = -1;
    info.uid = -1;
    info.protocols = protocols;


    filter = 0;

    clientCount = 0;
    gWebSocketServer = this;

    port = TPCLArgs::instance().arg(TPCLArgs::cliArgHttpPort, 10088).toInt();
    if (port < 1 || port > 65535)
        port = 10087;

    info.port = port;
    info.extensions = libwebsocket_get_internal_extensions();
    info.options = 0;

    context = libwebsocket_create_context(&info);

    Q_ASSERT(context);

    DEBUG() << "HTTP Serving at port: " << port;
}

TPWebSocketServer::~TPWebSocketServer()
{
    if (context)
        libwebsocket_context_destroy(context);
    delete filter;
}
/*
void TPWebSocketServer::timedServe()
{
    if (context)
        libwebsocket_service(context, 0);
}*/

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
        emit gWebSocketServer->clientConnected(gWebSocketServer->clientCount, wsi);
        DEBUG() << "WEBSOCKET: Connected. Total " << gWebSocketServer->clientCount << " clients";
    }
    else if (reason == LWS_CALLBACK_CLOSED)
    {
        --gWebSocketServer->clientCount;
        emit gWebSocketServer->clientDisconnected(gWebSocketServer->clientCount, wsi);
        DEBUG() << "WEBSOCKET: Disconnected. Total " << gWebSocketServer->clientCount << " clients";
    }
    else if (reason == LWS_CALLBACK_RECEIVE)
    {
        // todo: use fromRawData here to avoid copying.
        QByteArray b((const char *)in, len);
        emit gWebSocketServer->messageReceived(b, wsi);
    }
    else if (reason == LWS_CALLBACK_SERVER_WRITEABLE)
    {
        libwebsocket_service(context, 0);
        libwebsocket_callback_on_writable(context, wsi);
        DEBUG() << "WEBSOCKET: ServerWriteable: " << wsi;
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

    int fd = (int)(long)in;

    switch (reason)
    {
        case LWS_CALLBACK_HTTP_FILE_COMPLETION:
            DEBUG() << "HTTP COMPLETE: " << wsi;
        break;

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
            const char *http_request_path = (const char *)in;

            if (!http_request_path || strcmp(http_request_path, "/") == 0)
                http_request_path = "/topsecret/tunaplayer.html";

            QString filePath;
            bool found = false;
            for (int i=0;!found && i<gWebSocketServer->providers.count();++i)
            {
                filePath = gWebSocketServer->providers.at(i)->objectToFile(http_request_path);
                if (QFile::exists(filePath))
                    found = true;
            }

            if (!found)
            {
                ERROR() << "NOT FOUND: " << http_request_path;
                return 1;
            }

            QString contentType = gWebSocketServer->filenameToMime(filePath);
            DEBUG() << "HTTP: SERVE: file=" << filePath << " content:" << contentType;
            int status;
            if ((status = libwebsockets_serve_http_file(context, wsi, filePath.toUtf8().constData(), contentType.toUtf8().constData())) < 0)
            {
                ERROR() << "HTTP: SERVE: " << strerror(errno);
            }
    }
    break;

    case LWS_CALLBACK_ADD_POLL_FD:
    {
        DEBUG() << "HTTP: AddFD: " << fd;

        TPWebSocketServerNotifier *notifierRead =
                new TPWebSocketServerNotifier(fd,
                                              QSocketNotifier::Read,
                                              wsi,
                                              gWebSocketServer);
        TPWebSocketServerNotifier *notifierWrite =
                new TPWebSocketServerNotifier(fd,
                                              QSocketNotifier::Write,
                                              wsi,
                                              gWebSocketServer);

        notifierRead->setEnabled(true);
        notifierWrite->setEnabled(true);

        gWebSocketServer->notifiersRead.insert(fd, notifierRead);
        gWebSocketServer->notifiersWrite.insert(fd, notifierWrite);

        connect(notifierRead, SIGNAL(activated(int)), gWebSocketServer, SLOT(notifierActivated(int)));
        connect(notifierWrite, SIGNAL(activated(int)), gWebSocketServer, SLOT(notifierActivated(int)));
    }
    break;

    case LWS_CALLBACK_DEL_POLL_FD:
    {
        DEBUG() << "HTTP: RemoveFD: " << fd;
        gWebSocketServer->removeNotifier(fd);
    }
    break;

    default:
        break;
    }

    return 0;
}

TPWebSocketServerNotifier* TPWebSocketServer::readNotifierForFd(int fd)
{
    QMap<int, TPWebSocketServerNotifier *>::iterator it = notifiersRead.find(fd);
    if (it == notifiersRead.end())
        return NULL;

    TPWebSocketServerNotifier *notifier = it.value();
    return notifier;
}

TPWebSocketServerNotifier* TPWebSocketServer::writeNotifierForFd(int fd)
{
    QMap<int, TPWebSocketServerNotifier *>::iterator it = notifiersWrite.find(fd);
    if (it == notifiersWrite.end())
        return NULL;

    TPWebSocketServerNotifier *notifier = it.value();
    return notifier;
}

void TPWebSocketServer::notifierActivated(int fd)
{
    TPWebSocketServerNotifier* readNotifier = readNotifierForFd(fd);
    TPWebSocketServerNotifier* writeNotifier = writeNotifierForFd(fd);
    if (readNotifier && writeNotifier)
    {
        readNotifier->setEnabled(false);
        writeNotifier->setEnabled(false);
        libwebsocket_service(gWebSocketServer->context, 0);
        // Above call might cause the notifier to be deleted ->
        // refetch the pointer here.
        readNotifier = readNotifierForFd(fd);
        if (readNotifier)
            readNotifier->setEnabled(true);
        writeNotifier = writeNotifierForFd(fd);
        if (writeNotifier)
            writeNotifier->setEnabled(true);
    }
}

void TPWebSocketServer::sendFilteredEvent(TPWebSocketProtocolMessage message)
{
    QString eventId = message.id();

    QVector<void *>  targets;

    QMap<int, TPWebSocketServerNotifier *>::iterator it = notifiersWrite.begin();
    while (it != notifiersWrite.end())
    {
        TPWebSocketServerNotifier *notifier = it.value();
        void *clientHandle = notifier->getWsi();
        if (!message.eventFilter()->isEventFiltered(clientHandle, eventId))
            targets.append(clientHandle);

        ++it;
    }

    if (targets.count())
    {
        QByteArray content = message.serialize();

//        DEBUG() << "SEND-EVENT: " << content;

        char *buffer = new char [content.length() + LWS_SEND_BUFFER_PRE_PADDING + LWS_SEND_BUFFER_POST_PADDING];
        if (!buffer)
            return; // false maybe.

        memcpy(&buffer[LWS_SEND_BUFFER_PRE_PADDING], content.constData(), content.length());

        foreach(void *wsi, targets)
        {
            libwebsocket_write((struct libwebsocket *)wsi, (unsigned char *) &buffer[LWS_SEND_BUFFER_PRE_PADDING], content.length(), LWS_WRITE_TEXT);
        }

        delete [] buffer;
    }
}

void TPWebSocketServer::sendMessage(TPWebSocketProtocolMessage message)
{
    QByteArray content = message.serialize();

//    DEBUG() << "SEND-MESSAGE: " << content;

    char *buffer = new char [content.length() + LWS_SEND_BUFFER_PRE_PADDING + LWS_SEND_BUFFER_POST_PADDING];
    if (!buffer)
        return; // false maybe.

    memcpy(&buffer[LWS_SEND_BUFFER_PRE_PADDING], content.constData(), content.length());

    libwebsocket_write((struct libwebsocket *)message.getOrigin(), (unsigned char *) &buffer[LWS_SEND_BUFFER_PRE_PADDING], content.length(), LWS_WRITE_TEXT);

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

