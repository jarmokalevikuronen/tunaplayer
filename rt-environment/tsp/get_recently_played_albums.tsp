{
    "arguments":{
      "result-count": "6",
      "max-lastpltago": "604800"
    },
    "schemes":["album://"],
    "filterkeys": ["scheme","_pk_"],
    "filters":{
      "and":{
         "!equals" :["object://deleted", "value://1"],
         ">=" :["object://plcnt", "value://1"], 
         "<=" :["object://lastpltago", "parameter://max-lastpltago"]
      }
    },
    "sorting":[
      ["descending-num", "lastplt"]
    ]
}
