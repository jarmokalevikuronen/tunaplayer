# tunaPlayer #

**tunaPlayer** is music playback daemon that has a WWW UI (HTML5, CSS3) for controlling the functionality.

## Platforms ##

Primary target is **dreamplug**
(http://www.globalscaletechnologies.com/t-dreamplugdetails.aspx)
but it should run basically on any Linux platform having Qt (>=4.6) and mplayer available.

## Features in a nutshell ##

**Web radio**

**Local music (wav, mp3, wma, ogg, ..)**

**Playlists**

**Automatic album art search (lastFM) - Manual search to extent results not automatically found.**

**Podcasts (UI coming soon)**

## Compiling ##

### debian derivates ###

1. sudo apt-get install zlib1g-dev libtag1-dev mplayer libqt4-dev libasound2-dev

2. qmake -r tunaplayer.pro && make && sudo make install


## Usage ##

Operation requires quite new version of either Chrome, Firefox or Opera browser that has
WebSocket support.

Officially only Chromium is supported.


When running tunaPlayer locally, UI can be opened by browsing with the browser to URL

http://127.0.0.1:10088/topsecret/tunaplayer.html

