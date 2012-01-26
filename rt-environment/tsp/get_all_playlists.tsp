{
    "arguments":{},
    "schemes":["playlist://"],
    "filterkeys":["_pk_", "scheme", "fn"],
    "filters":{
      "and":{
         "!equals" :["object://category", "value://radio"],
         "!equals" :["object://deleted", "value://1"]
      }
    },
    "sorting":[
	["ascending", "name"]
    ]
}
