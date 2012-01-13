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

#ifndef TPSEARCHFILTERMATCHSTARTSWITH_H
#define TPSEARCHFILTERMATCHSTARTSWITH_H

#include "tpsearchfilteropmatchbase.h"
#include "tpsearchfilteropinterface.h"
#include <QString>

class TPSearchFilterMatchStartsWith : public TPSearchFilterOpMatchBase
{
public:

    TPSearchFilterMatchStartsWith(Qt::CaseSensitivity cs, bool inverse);

private:

    bool evaluate(TPSearchFilterEvalArgs &args);

private:

    Qt::CaseSensitivity caseSensitivity;
    bool inverse;
};

#endif // TPSEARCHFILTERMATCHSTARTSWITH_H
