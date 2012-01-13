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

#ifndef TPWEBSOCKETVIRTUALFOLDER_H
#define TPWEBSOCKETVIRTUALFOLDER_H

#include "tplibwebsocketinterface.h"
#include "tplibwebsocketinterface.h"

//! @class TPWebSocketVirtualFolder
//! @brief Handles mapping between filesystem and HTTP request paths.
class TPWebSocketVirtualFolder : public TPWebSocketDataProviderInterface
{
private: // From TPWebSocketDataProviderInterface

    const QString objectToFile(const char *objectPath);

public:
    TPWebSocketVirtualFolder(const QString _rootFolder, const QString _secret);

private:

    QString secret;
    QString secretWithPathDelims;
    QString rootFolder;
};

#endif // TPWEBSOCKETVIRTUALFOLDER_H
