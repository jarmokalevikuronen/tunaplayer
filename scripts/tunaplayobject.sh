#!/bin/bash
#
# Usage:
#  tunaplayobject.sh <type-of-object> <object-path>
#  Examples:
#    tunaplayobject.sh file /home/user/xxx/music.mp3
#    tunaplayobject.sh playlist /home/user/xxx/country.m3u
#    tunaplayobject.sh youtube https://youtube.com?watch&id=xxasyssas
#

if [ $# -lt 2 ]
then
  echo "TUNAERROR: ARGUMENTS: Usage : $0 type object"
  exit
fi

MPLAYER=`which mplayer`
#YOUTUBEDL=`which youtube-dl`
if [ $MPLAYER = "" ]
then
  echo "TUNAERROR: MISSING: mplayer"
  exit
fi
if [ ! -x $MPLAYER ]
then
  echo "TUNAERROR: NOTEXECUTABLE: mplayer"
  exit
fi

#if [ $YOUTUBEDL = "" ]
#then
#  echo "TUNAERROR: MISSING: youtube-dl"
#  exit
#fi
#if [ ! -x $YOUTUBEDL ]
#then
#  echo "TUNAERROR: NOTEXECUTABLE: youtube-dl"
#  exit
#fi

case "$1" in

file)
  $MPLAYER -slave -ao alsa "$2"
    ;;
playlist)
  $MPLAYER -slave -ao alsa -playlist "$2"
    ;;
youtube)
  $MPLAYER -slave -ao alsa -novideo -fs -cookies -cookies-file /tmp/tunayoutubecookie.txt $(/usr/bin/youtube-dl -g --cookies /tmp/tunayoutubecookie.txt "$2")
    ;;
*) echo  "TUNAERROR: ARGUMENTS"
   ;;
esac

