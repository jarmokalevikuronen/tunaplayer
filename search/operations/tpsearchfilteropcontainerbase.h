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

#ifndef TPSEARCHFILTEROPCONTAINERBASE_H
#define TPSEARCHFILTEROPCONTAINERBASE_H

#include <QVector>
#include <QString>
#include "tpsearchfilteropinterface.h"

class TPSearchFilterOpContainerBase : public TPSearchFilterOpInterface
{
public:

    TPSearchFilterOpContainerBase();
    virtual ~TPSearchFilterOpContainerBase();

    bool add(TPSearchFilterOpInterface *op);

    bool isContainer() const
    {
        return true;
    }

    bool addArg(const QString arg)
    {
        Q_UNUSED(arg);
        return false;
    }

    void preProcess(TPStoredProcedureArgs &args);

protected:

    QVector< TPSearchFilterOpInterface * > childs;
};

#endif // TPSEARCHFILTEROPCONTAINERBASE_H
