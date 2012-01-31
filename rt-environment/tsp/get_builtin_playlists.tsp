{
    "arguments":{},
    "schemes":["playlist://"],
    "filterkeys":["scheme", "_pk_", "fn", "category", "builtin"],
    "filters":{
      "and": {
         "!contains":["object://category", "value://radio"],
         "equals":["object://builtin", "value://1"]
      }
    },
    "sorting":[
	["descending", "name"]
    ]
}
