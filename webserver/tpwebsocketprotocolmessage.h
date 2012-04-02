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

#ifndef TPWEBSOCKETPROTOCOLMESSAGE_H
#define TPWEBSOCKETPROTOCOLMESSAGE_H

#include <QVariantMap>
#include "json.h"
#include <QDebug>
#include "tpwebsocketprotocoleventfilter.h"

using namespace QtJson;

//! @class TPWebSocketProtocolMessage
//! @brief Encapsulates a single web socket protocol JSON type of
//! message. Message can be either
//! 1. Command -> When received from Web Server
//! 2. Response -> Sent as a response to Command by the tunaplayer (=thiscode)
//! 3. Event -> Spontaneous state change type of message. E.g. "playback position changed"
class TPWebSocketProtocolMessage
{
public:

    TPWebSocketProtocolMessage();

    TPWebSocketProtocolMessage(TPWebSocketProtocolEventFilter *_filter);

    void initializeResponseTo(TPWebSocketProtocolMessage message);
    void initializeEvent(const QString id)
    {
        jsonData["type"] = "event";
        jsonData["id"] = id;
    }

    void initializeFromMap(QVariantMap map)
    {
        jsonData = map;
        args = map["args"].toMap();
    }

    void initializeFromSerialized(const QByteArray data)
    {
        bool ok = false;
        QVariant parsedMessage = Json::parse(data, ok);
        if (!ok)
        {
            qDebug() << "WEBSOCKET: PROTOCOL: Failed to parse json data: " << data.left(32);
            return;
        }

        initializeFromMap(parsedMessage.toMap());
    }

    void addArg(const QVariant key, const QVariant value)
    {
        args[key.toString()] = value;
    }

    void setArgs(const QVariantMap _args)
    {
        args = _args;
    }

    const QVariant arg(const QString key)
    {
        return args[key];
    }

    const QVariantMap arguments()
    {
        return args;
    }

    QByteArray serialize()
    {
        // Attach the arguments.
        jsonData["args"] = args;

        bool ok = false;
        return Json::serialize(jsonData, ok);
    }

    inline bool isOk() const
    {
        return isCommand() || isEvent() || isResponse();
    }

    inline bool isCommand() const
    {
        return jsonData["type"] == "command";
    }

    inline bool isEvent() const
    {
        return jsonData["type"] == "event";
    }

    inline bool isResponse() const
    {
        return jsonData["type"] == "response";
    }

    inline void setResponseStatus(const QString status, const QString description = "")
    {
        if (isResponse())
        {
            jsonData["status"] = status;
            if (description.length())
                jsonData["description"] = description;
        }
    }

    inline void setHeaderValue(const QString key, QVariant value)
    {
        jsonData[key] = value;
    }

    inline bool hasId() const
    {
        return id().length() > 0;
    }

    inline bool hasCookie() const
    {
        return cookie().length() > 0;
    }

    inline const QString id() const
    {
        return jsonData["id"].toString();
    }

    inline const QString cookie() const
    {
        return jsonData["cookie"].toString();
    }

    inline QVariantMap arguments() const
    {
        return jsonData["args"].toMap();
    }

    inline void setOrigin(void *_origin)
    {
        origin = _origin;
    }

    inline void* getOrigin() const
    {
        return origin;
    }

    inline TPWebSocketProtocolEventFilter* eventFilter() const
    {
        return filter;
    }

private:

    //! Currently active event filter(s) used to specify where to possibly send the message and where to not.
    //! This is only referred, not owned.
    TPWebSocketProtocolEventFilter *filter;

    //! Cookie value used to know where to send responses
    void *origin;

    //! Arguments of the protocol message - this is kept separate
    //! for conviniency/performance reasons until the data gets actually
    //! serialized for the "wire".
    QVariantMap args;

    //! Whole message as json data - not complete until serialize has been called.
    QVariantMap jsonData;
};

#endif // TPWEBSOCKETPROTOCOLMESSAGE_H
