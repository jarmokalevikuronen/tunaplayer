{
    "arguments":{
      "userprofile": "/"
    },
    "schemes":["album://"],
    "filterkeys": ["scheme","_pk_","artl","trcnt","len","plcnt","pllen","aadltries","updt","crtt","lastplt","len"],
    "filters":{
        "contains":["object://_d.usr", "parameter://userprofile"]
    },
    "sorting":[
	["ascending", "artist.name"],
	["ascending", "name"]
    ],
    "grouping-by": "artist.name"
}
