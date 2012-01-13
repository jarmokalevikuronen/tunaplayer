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

#include "tpsearchfilteropfactory.h"
#include "tpsearchfiltermatchequals.h"
#include "tpsearchfiltermatchcontains.h"
#include "tpsearchfiltermatchstartswith.h"
#include "tpsearchfiltermatchendswith.h"
#include "tpsearchfiltermatchcomparenum.h"
#include "tpsearchfilteropand.h"
#include "tpsearchfilteropor.h"
#include "tpsearchfilteropnand.h"

static const QString opEquals("equals");
static const QString opEqualsCs("Equals");
static const QString opNotEquals("!equals");
static const QString opNotEqualsCs("!Equals");

static const QString opContains("contains");
static const QString opContainsCs("Contains");
static const QString opNotContains("!contains");
static const QString opNotContainsCs("!Contains");

static const QString opStartsWith("startswith");
static const QString opStartsWithCs("Startswith");
static const QString opNotStartsWith("!startswith");
static const QString opNotStartsWithCs("!Startswith");

static const QString opEndsWith("endswith");
static const QString opEndsWithCs("Endswith");
static const QString opNotEndsWith("!endswith");
static const QString opNotEndsWithCs("!Endswith");

static const QString opGT("greaterthan");
static const QString opGT2(">");
static const QString opGET("greaterorequalthan");
static const QString opGET2(">=");
static const QString opIsEqual("equalthan");
static const QString opIsEqual2("==");
static const QString opSET("smallerorequalthan");
static const QString opSET2("<=");
static const QString opST("smallerthan");
static const QString opST2("<");

static const QString opAnd("and");
static const QString opAnd2("&&");
static const QString opOr("or");
static const QString opOr2("||");
static const QString opNand("!and");
static const QString opNand2("!&&");

TPSearchFilterOpInterface* TPSearchFilterOpFactory::create(const QString _name)
{
    QString name = _name.trimmed();

    if (name == opEquals)
        return new TPSearchFilterMatchEquals(Qt::CaseInsensitive, false);
    else if (name == opEqualsCs)
        return new TPSearchFilterMatchEquals(Qt::CaseSensitive, false);
    else if (name == opNotEquals)
        return new TPSearchFilterMatchEquals(Qt::CaseInsensitive, true);
    else if (name == opNotEqualsCs)
        return new TPSearchFilterMatchEquals(Qt::CaseSensitive, true);
    else if (name == opContains)
        return new TPSearchFilterMatchContains(Qt::CaseInsensitive, false);
    else if (name == opContainsCs)
        return new TPSearchFilterMatchContains(Qt::CaseSensitive, false);
    else if (name == opNotContains)
        return new TPSearchFilterMatchContains(Qt::CaseInsensitive, true);
    else if (name == opNotContainsCs)
        return new TPSearchFilterMatchContains(Qt::CaseSensitive, true);
    else if (name == opStartsWith)
        return new TPSearchFilterMatchStartsWith(Qt::CaseInsensitive, false);
    else if (name == opStartsWithCs)
        return new TPSearchFilterMatchStartsWith(Qt::CaseSensitive, false);
    else if (name == opNotStartsWith)
        return new TPSearchFilterMatchStartsWith(Qt::CaseInsensitive, true);
    else if (name == opNotStartsWithCs)
        return new TPSearchFilterMatchStartsWith(Qt::CaseSensitive, true);
    else if (name == opEndsWith)
        return new TPSearchFilterMatchEndsWith(Qt::CaseInsensitive, false);
    else if (name == opEndsWithCs)
        return new TPSearchFilterMatchEndsWith(Qt::CaseSensitive, false);
    else if (name == opNotEndsWith)
        return new TPSearchFilterMatchEndsWith(Qt::CaseInsensitive, true);
    else if (name == opNotEndsWithCs)
        return new TPSearchFilterMatchEndsWith(Qt::CaseSensitive, true);
    else if (name == opGT || name == opGT2)
        return new TPSearchFilterMatchCompareNum(TPSearchFilterMatchCompareNum::GreaterThan);
    else if (name == opGET || name == opGET2)
        return new TPSearchFilterMatchCompareNum(TPSearchFilterMatchCompareNum::GreaterOrEqualThan);
    else if (name == opIsEqual || name == opIsEqual2)
        return new TPSearchFilterMatchCompareNum(TPSearchFilterMatchCompareNum::EqualThan);
    else if (name == opSET || name == opSET2)
        return new TPSearchFilterMatchCompareNum(TPSearchFilterMatchCompareNum::SmallerOrEqualThan);
    else if (name == opST || name == opST2)
        return new TPSearchFilterMatchCompareNum(TPSearchFilterMatchCompareNum::SmallerThan);
    else if (name == opAnd || name == opAnd2)
        return new TPSearchFilterOpAnd();
    else if (name == opOr || name == opOr2)
        return new TPSearchFilterOpOr();
    else if (name == opNand || name == opNand2)
        return new TPSearchFilterOpNand();

    return NULL;
}
