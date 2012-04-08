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

#include "tpsearchfiltermatchcontains.h"
#include "tplog.h"

TPSearchFilterMatchContains::TPSearchFilterMatchContains(Qt::CaseSensitivity _caseSensitivity, bool _inverse)
{
    caseSensitivity = _caseSensitivity;
    inverse = _inverse;
}

bool TPSearchFilterMatchContains::evaluate(TPSearchFilterEvalArgs &args)
{
    QString v1(evalValue1(args));
    QString v2(evalValue2(args));
//    DEBUG() << "CONTAINS: v1=" << v1 << " v2=" << v2;
    bool comparison = v1.contains(v2, caseSensitivity);
    return inverse ? !comparison : comparison;
}

