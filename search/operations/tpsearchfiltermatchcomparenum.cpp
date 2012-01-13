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

#include "tpsearchfiltermatchcomparenum.h"


TPSearchFilterMatchCompareNum::TPSearchFilterMatchCompareNum(TPSearchFilterMatchCompareNum::CompareType _ct)
{
    ct = _ct;
}

bool TPSearchFilterMatchCompareNum::evaluate(TPSearchFilterEvalArgs &args)
{
    qint64 val1 = evalValue1Num(args);
    qint64 val2 = evalValue2Num(args);

    if (ct == GreaterThan)
        return val1 > val2;
    else if (ct == GreaterOrEqualThan)
        return val1 >= val2;
    else if (ct == EqualThan)
        return val1 == val2;
    else if (ct == SmallerOrEqualThan)
        return val1 <= val2;
    else if (ct == SmallerThan)
        return val1 < val2;

    return false;
}
