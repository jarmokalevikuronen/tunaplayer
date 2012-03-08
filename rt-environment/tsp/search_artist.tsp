{
    "arguments":{
      "criteria": ""
    },
    "schemes":["artist://"],
    "filterkeys": ["scheme","_pk_"],
    "filters":{
        "contains":["object://name", "parameter://criteria"]
    },
    "sorting":[
	["ascending", "name"]
    ]
}
