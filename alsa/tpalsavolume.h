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

#ifndef TPALSAVOLUME_H
#define TPALSAVOLUME_H

#include <QObject>
#include <alsa/asoundlib.h>

class TPALSAVolume : public QObject
{
    Q_OBJECT
public:
    explicit TPALSAVolume(QObject *parent = 0);
    ~TPALSAVolume();

    //! @brief Sets a volume to specified percents.
    bool setVolume(int percents);
    bool getVolume(int *percents);

signals:

public slots:

private: // New implementation

    bool init();
    void cleanup();

private:

    snd_mixer_t *mixerHandle;
    snd_mixer_selem_id_t *sElemId;
    snd_mixer_elem_t *masterElem;

    long minVolume;
    long maxVolume;

//    const char *card = "default";
//    const char *selem_name = "Master";
};

#endif // TPALSAVOLUME_H
