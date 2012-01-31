

function pendingRequest(cookie, callback) {
  this.cookie = cookie;
  this.callback = callback;
}

var tunaPlayer = {
  pendingRequests: {},
  cookie: 0,
  socket: null,
  onOpened: null,
  onOpenFailed: null,
  onClosed: null,
  onStartupProgress: null,
  onPlaybackControlsChanged: null,
  onPlaybackTrackChanged: null,
  onPlaybackAlbumChanged: null,
  onAlbumChanged: null,
  onPlaybackPositionChanged: null,
  onAlbumArtSearchItem: null,
  onAlbumArtSearchComplete: null,
  onVolumeChanged: null,
  onCurrentPlaylistChanged: null,
  onDatabaseChanged: null,
  openTimeoutTimer: null,
  cachedPlaybackControls: null,
  cachedPlaybackTrack: null,
  cachedPlaybackAlbum: null,
  cachedVolumeLevel: null,

  stringify: function(ctrl) {
    var s = "";
    if (ctrl["play"])
      s += "pl";
    if (ctrl["stop"])
      s+= "st";
    if (ctrl["pause"])
      s += "pa";
    if (ctrl["mute"])
      s += "mu";
    if (ctrl["unmute"])
      s += "un";
    if (ctrl["next"])
      s += "ne";

    return s;
  },

  open: function(url) {
    if (this.socket != null) {
      return true;
    }
    var protocol;
    var u = url;

    if (u.substring(0, 5) == "https") {
      protocol = "wss://";
      u = u.substr(8);
    } else {
      protocol = "ws://";
      if (u.substring(0, 4) == "http") {
        u = u.substr(7);
      }
    }
    u = u.split('/');
    var uri = protocol + u[0];

    try {
      this.socket = new WebSocket(uri, "tp-json");
    } catch (e) {
      console.log("Unable to create WebSocket, try MozWebSocket..");
      try {
        this.socket = new MozWebSocket(uri, "tp-json");
      } catch (e) {
        console.log("Unable to create MozWebSocket -> bailing out..");
      }
    }
 
    if (!this.socket)
      return false;

    this.openTimeoutTimer = setTimeout(function() { 
      tunaPlayer.openTimeoutTimer = null;
      if (tunaPlayer.socket.readyState != tunaPlayer.socket.OPEN) {
        if (tunaPlayer.onOpenFailed != null) {
          tunaPlayer.onOpenFailed();
        }
        tunaPlayer.socket.close();
        tunaPlayer.socket = null;
      }
    }, 5000);

    this.socket.onopen = function() {
      if (tunaPlayer.openTimeoutTimer) {
        clearTimeout(tunaPlayer.openTimeoutTimer);
        tunaPlayer.openTimeoutTimer = null;
      }
      if (tunaPlayer.onOpened) {
        tunaPlayer.onOpened();
        tunaPlayer.requestStatusUpdate();
      } else {
        alert("No onOpened handler");
      }
    };
    this.socket.onmessage = function(msg) {
      var json = JSON.parse(msg.data);
      var type = json["type"];
      if (type == "response") {
        var cookie = json["cookie"];
        if (cookie != undefined) {
          var request = tunaPlayer.pendingRequests[cookie];
          if (request != undefined) {
            if (request.callback) {
              request.callback(json);
            }
            delete tunaPlayer.pendingRequests[cookie];
          }
        }
      } else if (type == "event") {
        var id = json["id"];
        if (id == "startupprogress") {
          var percents = json["args"]["percent"];
          if (tunaPlayer.onStartupProgress && percents != undefined) {
            tunaPlayer.onStartupProgress(percents);
          }
        } else if (id == "playbackcontrolschanged") {
          tunaPlayer.cachedPlaybackControls = json["args"]["available"];
          if (tunaPlayer.onPlaybackControlsChanged) {
            tunaPlayer.onPlaybackControlsChanged(tunaPlayer.cachedPlaybackControls);
          }
        } else if (id == "playbackpositionchanged") {
          if (tunaPlayer.onPlaybackPositionChanged) {
            var position = json["args"];
            if (position != undefined) {
              tunaPlayer.onPlaybackPositionChanged(position);
            }
          }
        } else if (id == "playbacktrackchanged") {
          if (tunaPlayer.onPlaybackTrackChanged) {
            tunaPlayer.getCurrentTrack( function(json) {
              if (json["status"] == "OK" && json["args"] != undefined && json["args"]["items"] != undefined && json["args"]["items"].length > 0) {
                tunaPlayer.cachedPlaybackTrack = json["args"]["items"][0];
                tunaPlayer.onPlaybackTrackChanged(tunaPlayer.cachedPlaybackTrack);
              }
            });
          }
          if (tunaPlayer.onPlaybackAlbumChanged) {
            tunaPlayer.getCurrentAlbum( function(json) {
              if (json["status"] == "OK") {
                tunaPlayer.cachedPlaybackAlbum = json["args"]["items"][0];
                tunaPlayer.onPlaybackAlbumChanged(tunaPlayer.cachedPlaybackAlbum);
              }
            });
          }
        } else if (id == "albumartsearchitem") {
          if (tunaPlayer.onAlbumArtSearchItem) {
            tunaPlayer.onAlbumArtSearchItem(json["args"]);
          }
        } else if (id == "albumartsearchcomplete") {
          if (tunaPlayer.onAlbumArtSearchComplete) {
            tunaPlayer.onAlbumArtSearchComplete();
          }
        } else if (id == "volumechanged") {
          if (tunaPlayer.onVolumeChanged) {
            var percents = json["args"]["percents"];
            if (percents != undefined) {
               tunaPlayer.cachedVolumeLevel = percents;
               tunaPlayer.onVolumeChanged(percents); 
            }
          }
        } else if (id == "currentplaylistchanged") {
          if (tunaPlayer.onCurrentPlaylistChanged) {
            tunaPlayer.getCurrentPlaylist(function(json) {
              if (json["status"] == "OK") { 
                tunaPlayer.onCurrentPlaylistChanged(json);
              }
            });
          }
        } else if (id == "albumchanged") {
           if (tunaPlayer.onAlbumChanged) {
             tunaPlayer.onAlbumChanged(json["args"]);
           }
        } else if (id == "databasechanged") {
            if (tunaPlayer.onDatabaseChanged) {
              tunaPlayer.onDatabaseChanged();
            }
        }
      }
    }
    this.socket.onclose = function() {
      this.socket = null;
      if (tunaPlayer.onClosed) {
        tunaPlayer.onClosed();
      }
    };

    return true;
  },
  close: function() {
    if (this.socket != null) {
      this.socket.close();
      this.socket = null;
    }
  },
  cmdCreate: function(fname, callback) {
    var localCookie = this.setupCallback(callback);
    var cmd = {
      "type": "command",
      "id": fname,
      "cookie": localCookie
    };
    return cmd;
  },
  execSPCreate: function(spname, callback, args) {
    var localCookie = this.setupCallback(callback);
    var cmd = {
      "type": "command",
      "id": "execsp",
      "cookie": localCookie,
      "args": {
        "sp-name": spname,
        "sp-args": args
      }
    };
    
    return cmd;
  },
  callSP: function(spname, callback, args) {
    var cmd = this.execSPCreate(spname, callback, args);
    this.socket.send(JSON.stringify(cmd));
  },
  callFunc: function(fname, callback, args) {
    var cmd = this.cmdCreate(fname, callback);
    if (args != undefined && args != null) {
      cmd["args"] = args;
    }
    this.socket.send(JSON.stringify(cmd));
  },
  setupCallback: function(callback) {
    this.cookie += 1;
    this.pendingRequests[this.cookie] = new pendingRequest(this.cookie, callback);
    return this.cookie;
  },
  requestStatusUpdate: function() {
    this.callFunc("status");
  },
  playerControl: function(ctrl, callback) {
    this.callFunc("playercontrol", callback, {"code": ctrl});
  },
  play: function(callback) {
    this.playerControl("play", callback);
  },
  stop: function() {
    this.playerControl("stop");
  },
  pause: function() {
    this.playerControl("pause");
  },
  next: function() {
    this.playerControl("next");
  },
  seek: function(secs) {
    this.playerControl("seek:" + secs.toString());
  },
  seekPercents: function(percents) {
    this.playerControl("seek:" + percents.toString() + "%");
  },
  mute: function() {
    this.playerControl("mute");
  },
  unmute: function() {
    this.playerControl("unmute");
  },
  getCurrentAlbum: function(callback) {
    this.callSP("get_current_album", callback);
  },
  getCurrentTrack: function(callback) {
    this.callSP("get_current_track", callback);
  },
  getRadioStations: function(callback) {
    this.callSP("get_radio_stations", callback);
  },
  getFeeds: function(callback) {
    this.callSP("get_all_feeds", callback);
  },
  getFeedItems: function(id, callback) {
    this.callSP("get_feed_items", callback, { "id": id });
  },
  getAllArtists: function(callback) {
    this.callSP("get_all_artists", callback);
  },
  getAllAlbums: function(callback) {
    this.callSP("get_all_albums", callback);
  },
  //
  // Retrieves the currently playing artist information
  // Args:
  //  callback: function to be called once completed.
  //
  getCurrentArtist: function(callback) {
    this.callSP("get_current_artist", callback);
  },
  //
  // Retrieves the currently playing playlist.
  // Args:
  //  callback: functigon to be called once completed.
  //
  getCurrentPlaylist: function(callback) {
    this.callSP("get_current_playlist", callback);
  },
  getArtistAlbums: function(id, callback) {
    this.callSP("get_artist_albums", callback, { "artistid": id } );
  },
  getAlbum: function(id, callback) {
    this.callSP("get_album", callback, {"id": id} );
  },
  getAlbumTracks: function(id, callback) {
    this.callSP("get_album_tracks", callback, { "albumid": id } );
  },
  getPlaylists: function(callback) {
    this.callSP("get_all_playlists", callback);
  },
  addToPlaylist: function(id, callback) {
    this.addToPlaylistBack(id, callback);
  },
  addToPlaylistBack: function(id, callback) {
    this.callFunc("addtoplaylist", callback, { "id": id, "position": "back"  } );
  },
  addToPlaylistFront: function(id, callback) {
    this.callFunc("addtoplaylist", callback, { "id": id, "position": "front" } );
  },
  getArtistTracks: function(id, callback) {
    this.callSP("get_artist_tracks", callback, { "artistid": id } );
  },
  savePlaylist: function(plname, callback) {
    this.callFunc("saveplaylist", callback,{ "name": plname } );
  },
  clearPlaylist: function(callback) {
    this.callFunc("clearplaylist", callback);
  },
  removePlaylist: function(id, callback) {
    this.callFunc("removeplaylist", callback, {"id": id});
  },
  removeFromPlaylist: function(id, callback) {
    this.callFunc("removefromplaylist", callback, {"id": id});
  },
  selectPlaylist: function(id, callback) {
    this.callFunc("setactiveplaylist", callback, {"id": id});
  },
  shufflePlaylist: function(callback) {
    this.callFunc("shuffleplaylist", callback);
  },
  seekToTrack: function(id, callback) {
    this.callFunc("seektotrack", callback, {"id": id});
  },
  searchAlbumArt: function(id, callback) {
    this.callFunc("searchalbumart", callback, {"id": id});
  },
  searchAlbumArtCancel: function(callback) {
    this.callFunc("searchalbumartcancel", callback);
  },
  selectAlbumArt: function(id, callback) {
    this.callFunc("searchalbumartselect", callback, {"id": id});
  },
  setVolume: function(percents, callback) {
    this.callFunc("setvolume", callback, {"percents": percents });
  },
  reportVolume: function(callback) {
    this.callFunc("reportvolume", callback);
  },
  playPlaylist: function(name, callback) {
    this.selectPlaylist(name, function(json) {
      if (json["status"] == "OK") {
        tunaPlayer.play(callback);
      } else {
        callback(json);
      }
    });
  },
  getBuiltInPlaylists: function(callback) {
    this.callSP("get_builtin_playlists", callback);
  }
};


