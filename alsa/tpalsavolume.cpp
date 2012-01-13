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

TPALSAVolume::TPALSAVolume(QObject *parent) :
    QObject(parent)
{
    mixerHandle = 0;
    sElemId = 0;
    masterElem = 0;
    minVolume = 0;
    maxVolume = 0;

    if (!init())
        cleanup();
}

TPALSAVolume::~TPALSAVolume()
{
    cleanup();
}

bool TPALSAVolume::functional()
{
    return masterElem != 0 && maxVolume > minVolume && maxVolume;
}

bool TPALSAVolume::init()
{
    int status = 0;

    QString alsaCard =
            TPCLArgs::instance().arg(TPCLArgs::cliArgAlsaCardName, "default").toString();
    QString alsaControl =
            TPCLArgs::instance().arg(TPCLArgs::cliArgAlsaControlName, "Master").toString();

    DEBUG() << "ALSA: CARD=" << alsaCard << " CONTROL=" << alsaControl;

    // Create mixer element in order to find the master channel.
    int index = 0;
    snd_mixer_selem_id_alloca(&sElemId);
    snd_mixer_selem_id_set_index(sElemId, index);
    snd_mixer_selem_id_set_name(sElemId, alsaControl.toUtf8().constData());

    status = snd_mixer_open(&mixerHandle, 0);
    if (status != 0)
    {
        ERROR() << "ALSA: snd_mixer_open failed (" << status << ")";
        return false;
    }

    status = snd_mixer_attach(mixerHandle, alsaCard.toUtf8().constData());
    if (status != 0)
    {
        ERROR() << "ALSA: snd_mixer_attach failed (" << errno << ")";
        return false;
    }

    status = snd_mixer_selem_register(mixerHandle, NULL, NULL);
    if (status != 0)
    {
        ERROR() << "snd_mixer_selem_register failed (" << snd_strerror(status) << ")";
        return false;
    }

    status = snd_mixer_load(mixerHandle);
    if (status != 0)
    {
        ERROR() << "ALSA: snd_mixer_load failed (" << snd_strerror(status) << ")";
        return false;
    }

    masterElem = snd_mixer_find_selem(mixerHandle, sElemId);
    if (!masterElem)
    {
        ERROR() << "ALSA: snd_mixer_find_selem failed";
        return false;
    }

    status = snd_mixer_selem_get_playback_volume_range (masterElem, &minVolume, &maxVolume);
    if (status != 0)
    {
        ERROR() << "ALSA: snd_mixer_selem_get_playback_volume_range failed (" << snd_strerror(status) << ")";
        return false;
    }

    DEBUG() << "ALSA: VolumeRange " << minVolume << " -> " << maxVolume;

    return true;
}

bool TPALSAVolume::setVolume(int percents)
{
    if (!functional() || percents < 0 || percents > 100)
        return false;

    // -> Convert from percents to actual range.
    long range = maxVolume - minVolume;
    // Heh, heh 99.9%
    long value = minVolume + (percents / 99.9) * range;

    // Make sure volume is in bounds after all.
    value = qMin(maxVolume, value);
    value = qMax(minVolume, value);

    DEBUG() << "ALSA: Volume percents " << percents << " mapped to " << value << " in range(" << minVolume << ".." << maxVolume << ")";

    // Set the same volume for each end every imagineable channels
    for (int i=0;i<SND_MIXER_SCHN_LAST;i++)
    {
        snd_mixer_selem_set_playback_volume(masterElem, static_cast<snd_mixer_selem_channel_id_t>(i), value);
    }

    return true;
}

bool TPALSAVolume::getVolume(int *percents)
{
    if (!functional())
        return false;

    // The volume is get just from front left channel -> lets assume each and every
    // channel has the same volume in this case.
    long value = 0;
    int status = snd_mixer_selem_get_playback_volume(masterElem, SND_MIXER_SCHN_FRONT_LEFT, &value);
    if (status != 0)
    {
        ERROR() << "ALSA: snd_mixer_selem_get_playback_volume failed (" << snd_strerror(status) << ")";
        return false;
    }

    // make "zero based"
    value -= minVolume;
    long range = maxVolume - minVolume;

    int _percents = (int)((value * 100 + 1) / range);
    _percents = qMin((_percents), 100);
    _percents = qMax((_percents), 0);


    if (percents)
        *percents = _percents;

    DEBUG() << "ALSA: Volume: " << _percents << "%";

    return true;
}

void TPALSAVolume::cleanup()
{
    if (mixerHandle)
        snd_mixer_close(mixerHandle);
    mixerHandle = 0;
    masterElem = 0;
    sElemId = 0;
    minVolume = maxVolume = 0;
}



