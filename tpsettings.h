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

#ifndef TPSETTINGS_H
#define TPSETTINGS_H

#include <QObject>
#include <QSettings>

static const QString settingMaintainIntervalMinutes("maintainIntervalMinutes");
static const QString settingFeedUpdateIntervalSecs("feedUpdateIntervalSecs");
static const QString settingPlayTrackCmd("playTrackCmd");
static const QString settingPlayPlaylistCmd("playPlaylistCmd");
static const QString settingDisableAutoArtLoader("disableautoartloader");
static const QString settingWWWPort("port");
static const QString settingIpFilter("ipfilter");

class TPSettings : public QSettings
{
    Q_OBJECT

private:

    explicit TPSettings(const QString filename, QObject *parent = 0);

public:

    static void initialize(const QString filename, QObject *owner = 0);
    static void release();
    static TPSettings& instance();

    void set(const QString key, QVariant value);
    QVariant get(const QString key, QVariant defaultValue = QVariant());

signals:

    void settingChanged(const QString key);

private:

    static TPSettings *sett;
};

#endif // TPSETTINGS_H
