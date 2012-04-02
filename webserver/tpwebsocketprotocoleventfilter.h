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

#ifndef TPWEBSOCKETPROTOCOLEVENTFILTER_H
#define TPWEBSOCKETPROTOCOLEVENTFILTER_H

#include <QMap>
#include <QStringList>

//! @class TPWebSocketProtocolEventFilter
//! @brief Implements a event filtering capabilities
//! so that particular client can filter certain events
//! from not to be distributed to it.
class TPWebSocketProtocolEventFilter
{
public:
    TPWebSocketProtocolEventFilter();

    //! @brief Adds a specific event to be filtered for a specific client.
    void addFilteredEvent(void *clientHandle, const QString filteredEvent);

    //! @brief Removes a specific event filter for a specific client.
    void removeFilteredEvent(void *clientHandle, const QString removedEvent);

    //! @brief Checks whether specific event is filtered for a specific client.
    bool isEventFiltered(void *clientHandle, const QString event);

    //! @brief Cleans up all content specific for a client.
    void removeClient(void *clientHandle);

private:

    //! Client handle mapping to event(s).
    QMap<void *, QStringList> filterItems;
};

#endif // TPWEBSOCKETPROTOCOLEVENTFILTER_H
