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

#ifndef TPLOG_H
#define TPLOG_H

#include <QDebug>
#include <QThread>

#define PERF()          qDebug()        << "PERF    "
#define STATE()         qDebug()        << "STATE   "
#define DEBUG()         qDebug()        << "DEBUG   "
#define WARN()          qWarning()      << "WARNING "
#define ERROR()         qCritical()     << "ERROR   "

extern void tpMessageOutput(QtMsgType type, const char *msg);
extern void initDebugLogging();

#endif // TPLOG_H
