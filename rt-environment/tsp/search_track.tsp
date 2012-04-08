{
    "arguments":{
      "criteria": "",
      "userprofile": "/"
    },
    "schemes":["track://"],
    "filterkeys": ["scheme","_pk_"],
    "filters":{
        "and": {
          "contains":["object://name", "parameter://criteria"],
          "contains":["object://_d.usr", "parameter://userprofile"]
        }
    },
    "sorting":[
	["ascending", "name"]
    ]
}
