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

#ifndef TPPROTOCOLDEFINES_H
#define TPPROTOCOLDEFINES_H

static const QString protocolCommandStatus("status");
static const QString protocolCommandAddToPlaylist("addtoplaylist");
static const QString protocolCommandAddToPlaylistArgIdKey("id");
static const QString protocolCommandAddToPlaylistArgPositionKey("position");
static const QString protocolCommandAddToPlaylistArgPositionValueBack("back");
static const QString protocolCommandAddToPlaylistArgPositionValueFront("front");
static const QString protocolCommandClearPlaylist("clearplaylist");
static const QString protocolCommandPlayerControl("playercontrol");
static const QString protocolCommandPlayerControlArgCodeKey("code");
static const QString protocolCommandSearchAlbumArt("searchalbumart");
static const QString protocolCommandSearchAlbumArtArgIdKey("id");
static const QString protocolCommandSearchAlbumArtCancel("searchalbumartcancel");
static const QString protocolCommandSearchAlbumArtSelect("searchalbumartselect");
static const QString protocolCommandSearchAlbumArtSelectArgIdKey("id");

static const QString protocolCommandSavePlaylist("saveplaylist");
static const QString protocolCommandSavePlaylistArgNameKey("name");

static const QString protocolCommandRemovePlaylist("removeplaylist");
static const QString protocolCommandRemovePlaylistArgIdKey("id");

static const QString protocolCommandRemoveFromPlaylist("removefromplaylist");
static const QString protocolCommandRemoveFromPlaylistArgIdKey("id");

static const QString protocolCommandSetActivePlaylist("setactiveplaylist");
static const QString protocolCommandSetActivePlaylistArgIdKey("id");

static const QString protocolCommandShufflePlaylist("shuffleplaylist");

static const QString protocolCommandSeekToTrack("seektotrack");
static const QString protocolCommandSeekToTrackArgIdKey("id");

//! This command will effectively shutdown the whole application.
static const QString protocolCommandQuit("quit");

//! Volume Controlling commands
static const QString protocolCommandSetVolume("setvolume");
static const QString protocolCommandSetVolumeArgPercentsKey("percents");
//! Calling this method will cause a volumechanged event to be reported.
static const QString protocolCommandReportVolume("reportvolume");

//! Can be used to disable certain events from being reported (E.g. no playback progress
//! reporting when the currently visible view within the UI does not show such information).
static const QString protocolCommandDisableEvents("disableevents");

// Generic status reports.
static const QString protocolExecStatusOK("OK");
static const QString protocolExecStatusError("FAIL");
static const QString protocolExecErrorDescriptionNotReady("NotReady");
static const QString protocolExecErrorDescriptionUnknownSP("UnknownSP");
static const QString protocolExecErrorDescriptionInternalError("InternalError");
static const QString protocolExecErrorDescriptionArgument("InvalidArguments");
// COMMAND: execsp
static const QString protocolCommandExecSP("execsp");
// REQUEST ARGUMENTS FOR execSP - given in the root object
static const QString protocolCommandExecSPArgNameKey("sp-name");
static const QString protocolCommandExecSPArgArgumentsKey("sp-args");
// RESPONSE ARGUMENTS FOR execSP - given in the root object
static const QString protocolCommandExecSPResponseArgNameKey("sp-name");
static const QString protocolCommandExecSPResponseArgArgsKey("sp-args-in");
static const QString protocolCommandExecSPResponseArgItemsKey("items");
static const QString protocolCommandExecSPResponseArgItemKey("item");

// Notified when new tracks wre found -> it is time for UI to do a refresh.
static const QString protocolEventDatabaseChanged("databasechanged");

static const QString protocolEventPlaybackTrackChanged("playbacktrackchanged");
static const QString protocolEventPlaybackAlbumChanged("playbackalbumchanged");

static const QString protocolEventPlaybackPositionChanged("playbackpositionchanged");
static const QString protocolEventPlaybackPositionChangedArgPositionSecondsKey("position");
static const QString protocolEventPlaybackPositionChangedArgPositionPercentsKey("percents");

static const QString protocolEventPlaybackControlsChanged("playbackcontrolschanged");
static const QString protocolEventPlaybackControlsChangedArgControlsKey("available");

static const QString protocolEventAlbumArtSearchItem("albumartsearchitem");
static const QString protocolEventAlbumArtSearchItemArgIdKey("id");
static const QString protocolEventAlbumArtSearchItemArgArtKey("art");
static const QString protocolEventAlbumArtSearchComplete("albumartsearchcomplete");

static const QString protocolEventStartupProgress("startupprogress");
static const QString protocolEventStartupProgressArgPercents("percent");

//! Playback volume level. Emitted on demand, and if/when change is detected otherwise.
static const QString protocolEventVolumeChanged("volumechanged");
static const QString protocolEventVolumeChangedArgPercentsKey("percents");

static const QString protocolEventAlbumChanged("albumchanged");

// Reported when current playlist has changed.
static const QString protocolEventCurrentPlaylistChanged("currentplaylistchanged");

#endif // TPPROTOCOLDEFINES_H
