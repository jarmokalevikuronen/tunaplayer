{
    "arguments":{
      "albumid": ""
    },
    "schemes":["track://"],
    "filterkeys": ["scheme","_pk_", "fn", "arid", "alid"],
    "filters":{
        "equals":["object://album.id", "parameter://albumid"]
    },
    "sorting":[
	["ascending-num", "idx"],
	["ascending", "name"]
    ]
}
