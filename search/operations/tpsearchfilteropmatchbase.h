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

#ifndef TPSEARCHFILTEROPMATCHBASE_H
#define TPSEARCHFILTEROPMATCHBASE_H

#include "tpsearchfilteropinterface.h"
#include <QString>


class TPSearchFilterOpMatchBase : public TPSearchFilterOpInterface
{
protected:

    TPSearchFilterOpMatchBase();

    const QString evalValue1(TPSearchFilterEvalArgs &args);
    qint64 evalValue1Num(TPSearchFilterEvalArgs &args);
    const QString evalValue2(TPSearchFilterEvalArgs &args);
    qint64 evalValue2Num(TPSearchFilterEvalArgs &args);

private:



    class Value
    {
    public:

        enum ValueTarget
        {
            ValueTargetInvalid,
            // value://Rock
            ValueTargetValue,
            // parameter://someargname
            ValueTargetParameter,
            // object://lastplayedtime
            ValueTargetObject
        };

    public:

        Value()
        {
            scheme = ValueTargetInvalid;
        }

        bool parse(const QString source);
        inline bool isSetUp() const
        {
            return scheme != ValueTargetInvalid;
        }

        ValueTarget scheme;
        QString value;
    };

    Value value1;
    Value value2;

    const QString evalValue(Value &valRef, TPSearchFilterEvalArgs &args);
    qint64 evalValueNum(Value &valRef, TPSearchFilterEvalArgs &args);

private:

    bool add(TPSearchFilterOpInterface */*op*/)
    {
        return false;
    }


    bool isContainer() const
    {
        return false;
    }

public:

    virtual ~TPSearchFilterOpMatchBase(){;}

    bool addArg(const QString arg);
};

#endif // TPSEARCHFILTEROPMATCHBASE_H
