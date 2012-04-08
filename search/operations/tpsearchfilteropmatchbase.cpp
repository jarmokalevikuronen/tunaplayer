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

#include "tpsearchfilteropmatchbase.h"
#include "tpschemes.h"
#include "tpsearchfilterevalargs.h"

TPSearchFilterOpMatchBase::TPSearchFilterOpMatchBase()
{
}

bool TPSearchFilterOpMatchBase::addArg(const QString arg)
{
    if (!value1.isSetUp())
        return value1.parse(arg);
    else if (!value2.isSetUp())
        return value2.parse(arg);

    return false;
}

const QString TPSearchFilterOpMatchBase::evalValue(Value &valRef, TPSearchFilterEvalArgs &args)
{
    if (valRef.scheme == Value::ValueTargetObject)
        return args.object()->getString(valRef.value);
    else if (valRef.scheme == Value::ValueTargetParameter)
        return args.args()->argValue(valRef.value);
    else if (valRef.scheme == Value::ValueTargetValue)
        return valRef.value;

    return QString();
}

int TPSearchFilterOpMatchBase::evalValueNum(Value &valRef, TPSearchFilterEvalArgs &args)
{    
    if (valRef.cachedNumberAvailable)
        return valRef.cachedNumber;

    return evalValue(valRef, args).toInt();
}

void TPSearchFilterOpMatchBase::preProcess(TPStoredProcedureArgs &args)
{
    if (value1.isSetUp())
        value1.preProcess(args);
    if (value2.isSetUp())
        value2.preProcess(args);
}

bool TPSearchFilterOpMatchBase::Value::parse(const QString line)
{
    if (line.startsWith(schemeValue))
    {
        value = line.mid(schemeValue.length());
        scheme = ValueTargetValue;
    }
    else if (line.startsWith(schemeParameter))
    {
        value = line.mid(schemeParameter.length());
        scheme = ValueTargetParameter;
    }
    else if (line.startsWith(schemeObject))
    {
        value = line.mid(schemeObject.length());
        scheme = ValueTargetObject;
    }

    return scheme != ValueTargetInvalid;
}

void TPSearchFilterOpMatchBase::Value::preProcess(TPStoredProcedureArgs &args)
{
    cachedNumberAvailable = cachedStringAvailable = false;

    if (scheme == ValueTargetParameter)
    {
        cachedString = args.argValue(value);
        cachedStringAvailable = cachedString.length() > 0;

        cachedNumber = cachedString.toInt(&cachedNumberAvailable);
    }
    else if (scheme == ValueTargetValue)
    {
        cachedString = value;
        cachedStringAvailable = cachedString.length() > 0;

        cachedNumber = cachedString.toInt(&cachedNumberAvailable);
    }
}

