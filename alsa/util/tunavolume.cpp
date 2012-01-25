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

#include <alsa/asoundlib.h>

//! @class TPALSAVolume
//! @brief Simple class used to get/set volume level.
class TPALSAVolume
{
public:

    explicit TPALSAVolume();
    ~TPALSAVolume();

    //! @brief Sets a volume to specified percents.
    bool setVolume(const char *card, const char *control, int percents);
    bool getVolume(const char *card, const char *control, int *percents);

private: // New implementation

    bool init(const char *card, const char *control);
    void cleanup();

private:

    snd_mixer_t *mixerHandle;
    snd_mixer_selem_id_t *sElemId;
    snd_mixer_elem_t *masterElem;

    long minVolume;
    long maxVolume;
};


TPALSAVolume::TPALSAVolume()
{
    mixerHandle = 0;
    sElemId = 0;
    masterElem = 0;
    minVolume = 0;
    maxVolume = 0;
}

TPALSAVolume::~TPALSAVolume()
{
    cleanup();
}

bool TPALSAVolume::init(const char *alsaCard, const char *alsaControl)
{
    int status = 0;

    // Create mixer element in order to find the master channel.
    int index = 0;
    snd_mixer_selem_id_alloca(&sElemId);
    snd_mixer_selem_id_set_index(sElemId, index);
    snd_mixer_selem_id_set_name(sElemId, alsaControl);

    status = snd_mixer_open(&mixerHandle, 0);
    if (status != 0)
    {
        return false;
    }

    status = snd_mixer_attach(mixerHandle, alsaCard);
    if (status != 0)
    {
        return false;
    }

    status = snd_mixer_selem_register(mixerHandle, NULL, NULL);
    if (status != 0)
    {
        return false;
    }

    status = snd_mixer_load(mixerHandle);
    if (status != 0)
    {
        return false;
    }

    masterElem = snd_mixer_find_selem(mixerHandle, sElemId);
    if (!masterElem)
    {
        return false;
    }

    status = snd_mixer_selem_get_playback_volume_range (masterElem, &minVolume, &maxVolume);
    if (status != 0)
    {
        return false;
    }

    return true;
}

bool TPALSAVolume::setVolume(const char *alsaCard, const char *alsaControl, int percents)
{
    if (!init(alsaCard, alsaControl))
        return false;

    // -> Convert from percents to actual range.
    long range = maxVolume - minVolume;
    // Heh, heh 99.9%
    long value = minVolume + (percents / 99.9) * range;

    // Make sure volume is in bounds after all.
    if (value > maxVolume) value = maxVolume;
    if (value < minVolume) value = minVolume;

    snd_mixer_selem_set_playback_volume_all(masterElem, value);

    cleanup();

    return true;
}

bool TPALSAVolume::getVolume(const char *alsaCard, const char *alsaControl, int *percents)
{
    if (!init(alsaCard, alsaControl))
        return false;

    // The volume is get just from front left channel -> lets assume each and every
    // channel has the same volume in this case.
    long value = 0;
    int status = snd_mixer_selem_get_playback_volume(masterElem, SND_MIXER_SCHN_FRONT_LEFT, &value);
    if (status != 0)
    {
        cleanup();
        return false;
    }

    // make "zero based"
    value -= minVolume;
    long range = maxVolume - minVolume;

    int _percents = (int)((value * 100 + 1) / range);
    if (_percents < 0) _percents = 0;
    if (_percents > 100) _percents = 100;

    if (percents)
        *percents = _percents;

    cleanup();
    return true;
}

void TPALSAVolume::cleanup()
{
    if (mixerHandle)
    {
        snd_mixer_free(mixerHandle);
        snd_mixer_close(mixerHandle);
    }
    mixerHandle = 0;
    masterElem = 0;
    sElemId = 0;
    minVolume = maxVolume = 0;
}


int main(int argc, char *argv[])
{
  TPALSAVolume volume;

  if (argc == 4 && strcmp(argv[1], "get") == 0)
  {
      int percents = 0;
      bool ok = volume.getVolume(argv[2], argv[3], &percents);
      fprintf(stdout, "%d", ok ? percents : -1);
      fflush(stdout);
      return 0;
  }
  else if (argc == 5 && strcmp(argv[1], "set") == 0)
  {
      int percents = atoi(argv[2]);
      if (percents < 0) percents = 0;
      if (percents > 100) percents = 100;
      bool ok = volume.setVolume(argv[3], argv[4], percents);
      fprintf(stdout, "%s", ok ? "OK" : "ERROR");
      fflush(stdout);
      return 0;
  }   

  return 1;
}

