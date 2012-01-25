/*
This file is part of tunaplayer project
Copyright (C) 2012  Jarmo K. Kuronen <jarmok@iki.fi>

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
#include <errno.h>
#include "tpalsavolume.h"
#include "tpclargs.h"
#include "tplog.h"
#include <stdio.h>

TPALSAVolume::TPALSAVolume()
{
}

TPALSAVolume::~TPALSAVolume()
{
}

bool TPALSAVolume::setVolume(int percents)
{
    QString cli;

    cli = "/usr/bin/tunavolume set ";
    cli += QString::number(percents);
    cli += " \"" + TPCLArgs::instance().arg(TPCLArgs::cliArgAlsaCardName, "default").toString() + "\"";
    cli += " \"" + TPCLArgs::instance().arg(TPCLArgs::cliArgAlsaControlName, "Master").toString() + "\"";
DEBUG() << "Executing: " << cli;
    FILE *f = popen(cli.toUtf8().constData(), "r");
    if (!f)
        return false;

    QString response;
    char line[12];
    while (fgets(line, sizeof(line), f))
    {
        response += line;
    }
    pclose(f);

    DEBUG() << "ALSA::setVolume response: " << response;

    return response == "OK";
}

bool TPALSAVolume::getVolume(int *percents)
{
    QString cli;

    cli = "/usr/bin/tunavolume get";
    cli += " \"" + TPCLArgs::instance().arg(TPCLArgs::cliArgAlsaCardName, "default").toString() + "\"";
    cli += " \"" + TPCLArgs::instance().arg(TPCLArgs::cliArgAlsaControlName, "Master").toString() + "\"";

    FILE *f = popen(cli.toUtf8().constData(), "r");
    if (!f)
        return false;

    QString response;
    char line[12];
    while (fgets(line, sizeof(line), f))
    {
        response += line;
    }
    pclose(f);

    DEBUG() << "ALSA::getVolume response: " << response;

    bool ok = false;
    *percents = response.toInt(&ok);
    return ok;
}


