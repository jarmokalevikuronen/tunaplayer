{
    "arguments":{
      "result-count": "6"
    },
    "schemes":["album://"],
    "filterkeys": ["scheme","_pk_"],
    "filters":{
      "and":{
         "!equals" :["object://deleted", "value://1"]
      }
    },
    "sorting":[
      ["descending-num", "plcnt"]
    ]
}
