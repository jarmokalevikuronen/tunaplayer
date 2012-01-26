{
    "arguments":{
      "artistid": ""
    },
    "schemes":["album://"],
    "filterkeys": ["scheme","_pk_","artl","trcnt","len","plcnt","pllen","aadltries","updt","crtt","lastplt","len"],
    "filters":{
        "equals":["object://artist.id", "parameter://artistid"]
    },
    "sorting":[
	["ascending", "year"],
	["ascending", "name"]
    ]
}
