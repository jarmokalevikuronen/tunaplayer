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

#include <QSocketNotifier>
#include <QObject>
#include <QDebug>
#include <QTimer>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "tpsignalhandler.h"
#include "tplog.h"

// STATIC DATA INITIALIZATION
int TPSignalHandler::termFd[2];
int TPSignalHandler::intFd[2];

TPSignalHandler::TPSignalHandler(QObject *parent) :
    QObject(parent)
{
    termN = intN = 0;

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, termFd))
    {
	ERROR() << "CORE: socketpair failed " << strerror(errno);
        return;
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, intFd))
    {
	ERROR() << "CORE: socketpair failed " << strerror(errno);
        return;
    }

    termN = new QSocketNotifier(termFd[1], QSocketNotifier::Read, this);
    connect(termN, SIGNAL(activated(int)), this, SLOT(handleTerm()));

    intN = new QSocketNotifier(intFd[1], QSocketNotifier::Read, this);
    connect(intN, SIGNAL(activated(int)), this, SLOT(handleInt()));

    struct sigaction term;
    term.sa_handler = termHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags = SA_RESTART;

    if (sigaction(SIGTERM, &term, 0) > 0)
    {
	ERROR() << "CORE: Failed to install SIGHTERM handler " << strerror(errno);
    }

    struct sigaction intr;
    intr.sa_handler = intHandler;
    sigemptyset(&intr.sa_mask);
    intr.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &intr, 0) > 0)
    {
	ERROR() << "CORE: Failed to install SIGINT handler " << strerror(errno);
    }
}

TPSignalHandler::~TPSignalHandler()
{
}

void TPSignalHandler::termHandler(int)
{
    char a = 1;

    if (::write(termFd[0], &a, sizeof(a)) < 0)
    {
	ERROR() << "CORE: Failed to write to term socket " << strerror(errno);
    }
}

void TPSignalHandler::intHandler(int)
{
    char a = 1;

    if (::write(intFd[0], &a, sizeof(a)) < 0)
    {
	ERROR() << "CORE: Failed to write to int socket " << strerror(errno);
    }
}

void TPSignalHandler::handleTerm()
{
    termN->setEnabled(false);

    char tmp;

    if (::read(termFd[1], &tmp, sizeof(tmp)) < 0)
    {
        ERROR() << "CORE: Failed to read sig term fd " << strerror(errno);
    }

    DEBUG() << "CORE: Received TERM signal";

    emit quit();

    termN->setEnabled(true);
}

void TPSignalHandler::handleInt()
{
    intN->setEnabled(false);

    char tmp;

    if (::read(intFd[1], &tmp, sizeof(tmp)) < 0)
    {
        ERROR() << "CORE: Failed to read int fd " << strerror(errno);
    }

    DEBUG() << "CORE: Received INT signal";

    emit quit();

    intN->setEnabled(true);
}
