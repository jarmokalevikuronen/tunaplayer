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

#include "tpclargs.h"
#include "tplog.h"

// STATIC DATA INITIALIZATION
TPCLArgs *TPCLArgs::_instance = 0;
const char *TPCLArgs::cliArgHttpPort = "-p";
const char *TPCLArgs::cliArgAlsaCardName = "-volacard";
const char *TPCLArgs::cliArgAlsaControlName = "-volactrl";


void TPCLArgs::initialize(QStringList cliArgs)
{
    DEBUG() << "INIT: === CLI ARGUMENTS ===";

    if (_instance)
        return;
    _instance = new TPCLArgs;
    if (!_instance)
        return;

    for (int i=1;i<cliArgs.count();i++)
    {
        QString argKey = cliArgs.at(i);
        if (argKey.startsWith("-"))
        {
            i++;
            if (i < cliArgs.count())
            {
                QString value = cliArgs.at(i);

                DEBUG() << "INIT:    " << argKey << " = " << value;

                _instance->args[argKey] = value;
            }
        }
    }

    DEBUG() << "INIT: =====================";
}

TPCLArgs& TPCLArgs::instance()
{
    Q_ASSERT(_instance);
    return *_instance;
}

void TPCLArgs::release()
{
    delete _instance; _instance = 0;
}

QVariant TPCLArgs::arg(const char *key, QVariant def)
{
    if (!key && !(*key))
        return def;

    QString _key(key);
    if (!args.contains(_key))
        return def;

    return args[_key];
}
