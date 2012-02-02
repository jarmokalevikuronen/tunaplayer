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

#include "tpplaybackstatisticscollector.h"
#include "tputils.h"
#include "tptrack.h"
#include "tpassociativemeta.h"

TPPlaybackStatisticsCollector::TPPlaybackStatisticsCollector(QObject *parent) :
    QObject(parent)
{
    secondSum = 0;
    previousSecondValue = -1;
    currTrack = 0;
}

TPPlaybackStatisticsCollector::~TPPlaybackStatisticsCollector()
{
    if (currTrack)
        currTrack->dec();
}

void TPPlaybackStatisticsCollector::currentTrackChanged(TPTrack *track)
{
    if (currTrack != track)
    {
        // track changed. Update the statistics regarding
        // the old track whom playback was stopped.
        if (currTrack)
        {
            // From a real track to non real track.
            if (secondSum > currTrack->getLen()/3)
            {
                // Inc playcount and mark last played timestamp.
                currTrack->incPlayCount();
                currTrack->setInt(objectAttrLastPlayed, TPUtils::currentEpoch());
                currTrack->save(100);
            }
            else if (secondSum > 10)
            {
                currTrack->incIntValue(objectAttrShortPlays);
                currTrack->save(100);
            }

            currTrack->dec();
            currTrack = 0;
        }
    }
    else
        return;

    currTrack = track;
    if (currTrack)
    {
        currTrack->inc();
        secondSum = 0;
        previousSecondValue = -1;
    }
}

void TPPlaybackStatisticsCollector::playbackPositionChanged(TPTrack *track, int seconds)
{
    if (!track || track != currTrack)
        return;

    if (seconds == (previousSecondValue+1) || seconds == (previousSecondValue+2))
        secondSum += (seconds - previousSecondValue);
    else
        previousSecondValue = seconds;
}
