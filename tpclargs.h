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

#ifndef TPCLARGS_H
#define TPCLARGS_H

#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QString>

//! @class TPCLArgs
//! @brief Class that defines the command line arguments
class TPCLArgs
{
public:

    //! Key for command line argument that defines the http port.
    static const char *cliArgHttpPort;
    static const char *cliArgAlsaCardName;
    static const char *cliArgAlsaControlName;
    static const char *cliArgMediaPath;
    static const char *cliArgLogLevel;
    static const char *cliArgSecret;
    static const char *cliArgMaintainInterval;

    static TPCLArgs& initialize(QStringList cliArgs);
    static TPCLArgs& instance();
    static void release();

    bool usageRequested();

    QVariant arg(const char *key, QVariant def = QVariant());

    QString getUsageText();

private:

    //! Flags set to true if usage is requested
    bool usage;

    //! List of command line arguments.
    QMap<QString, QVariant> args;

    //! Singleton
    static TPCLArgs *_instance;
};

#endif // TPCLARGS_H
