#/bin/sh

/usr/bin/mplayer -novideo -ao alsa -slave -fs -cookies -cookies-file /tmp/cookie.txt $(/usr/bin/youtube-dl -g --cookies /tmp/cookie.txt $1)
