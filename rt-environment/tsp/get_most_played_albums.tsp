{
    "arguments":{
      "result-count": "6"
    },
    "schemes":["album://"],
    "filterkeys": ["scheme","_pk_"],
    "filters":{
      "and":{
         ">=" :["object://plcnt", "value://1"],
         "!equals" :["object://deleted", "value://1"]
      }
    },
    "sorting":[
      ["descending-num", "plcnt"],
      ["descending-num", "pllen"],
      ["ascending", "artist.name"],
      ["ascending", "name"]
    ]
}
