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
const char *TPCLArgs::cliArgMediaPath = "-media";
const char *TPCLArgs::cliArgLogLevel = "-loglevel";
const char *TPCLArgs::cliArgSecret = "-secret";
const char *TPCLArgs::cliArgMaintainInterval = "-maintain";
const char *TPCLArgs::cliArgIpFilter = "-ipfilter";
const char *TPCLArgs::cliArgMouseRemco = "-remco";
const char *TPCLArgs::cliArgRemcoMapping = "-remcoconfig";

static struct usage_item
{
    const char *key;
    const char *description;
}usageItems[] =
{
    {TPCLArgs::cliArgHttpPort, "Port to listen. (10088)."},
    {TPCLArgs::cliArgAlsaCardName, "ALSA Card for volume control. (default)"},
    {TPCLArgs::cliArgAlsaControlName, "ALSA Control for volume control. (Master)"},
    {TPCLArgs::cliArgLogLevel, "Logging level - oneof {none,debug,warning,error} (none)"},
    {TPCLArgs::cliArgMaintainInterval, "How often, in minutes, the filesystem is scanned to detect changes. (30)"},
    {TPCLArgs::cliArgMediaPath, "Where to search for music content. Multiple paths can be given by separating those with \';\'. ($HOME)."},
    {TPCLArgs::cliArgSecret, "Secret value for http server. (topsecret). Server will serve in http://ip:port/<secret>/tunaplayer.html"},
    {TPCLArgs::cliArgIpFilter, "IP Mask as regular expression. Used to specify which devices are allowed to connect to this server (127.0.0.1)"},
    {TPCLArgs::cliArgMouseRemco, "Input device for mouse remote control. (E.g. /dev/input/event5). (not enabled)."},
    {TPCLArgs::cliArgRemcoMapping, "Remote control config file in json. (Not enabled)." },
    {0, 0}
};

TPCLArgs& TPCLArgs::initialize(QStringList cliArgs)
{
    DEBUG() << "INIT: === CLI ARGUMENTS ===";

    if (_instance)
        return *_instance;

    _instance = new TPCLArgs;
    Q_ASSERT(_instance);

    _instance->usage = false;
    if (cliArgs.count() == 2 && (cliArgs.at(1) == "-help" || cliArgs.at(1) == "--help" || cliArgs.at(1) == "help" || cliArgs.at(1) == "-?" || cliArgs.at(1) == "?"))
        _instance->usage = true;
    else
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
    return *_instance;
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

bool TPCLArgs::usageRequested()
{
    return usage;
}

QString TPCLArgs::getUsageText()
{
    QString text = "Usage:\n\n";
    int i = 0;
    while (usageItems[i].key)
    {
        QString line;
        line += "  ";
        line += usageItems[i].key;
        while (line.length() < 20)
            line.append(" ");
        line += usageItems[i].description;
        line += "\n";
        text += line;
        ++i;
    }

    return text;
}
