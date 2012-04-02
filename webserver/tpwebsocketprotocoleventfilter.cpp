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

#include "tpwebsocketprotocoleventfilter.h"
#if 0
#include "tplog.h"
#endif

TPWebSocketProtocolEventFilter::TPWebSocketProtocolEventFilter()
{
}

void TPWebSocketProtocolEventFilter::addFilteredEvent(void *clientHandle, const QString filteredEvent)
{
    QStringList list = filterItems.value(clientHandle);
    if (!list.contains(filteredEvent))
        list.append(filteredEvent);
    filterItems.insert(clientHandle, list);

#if 0
    DEBUG() << "ZZZZZZ ADD EVENT FILTER " << (int)clientHandle;
    QMap<void *, QStringList>::iterator it = filterItems.begin();
    while (it != filterItems.end())
    {
        DEBUG() << "object: " << (int)it.key() << " = " << it.value();
        ++it;
    }
#endif
}

void TPWebSocketProtocolEventFilter::removeFilteredEvent(void *clientHandle, const QString removedEvent)
{
    QStringList list = filterItems.value(clientHandle);
    list.removeAll(removedEvent);
    filterItems.insert(clientHandle, list);

#if 0
    DEBUG() << "ZZZZZZ REMOVE EVENT FILTER " << (int)clientHandle;
    QMap<void *, QStringList>::iterator it = filterItems.begin();
    while (it != filterItems.end())
    {
        DEBUG() << "object: " << (int)it.key() << " = " << it.value();
        ++it;
    }
#endif
}

bool TPWebSocketProtocolEventFilter::isEventFiltered(void *clientHandle, const QString event)
{
    return filterItems.value(clientHandle).contains(event);
}

void TPWebSocketProtocolEventFilter::removeClient(void *clientHandle)
{
    filterItems.remove(clientHandle);

#if 0
    DEBUG() << "ZZZZZZ REMOVE CLIENT";
    QMap<void *, QStringList>::iterator it = filterItems.begin();
    while (it != filterItems.end())
    {
        DEBUG() << "object: " << (int)it.key() << " = " << it.value();
        ++it;
    }
#endif
}

