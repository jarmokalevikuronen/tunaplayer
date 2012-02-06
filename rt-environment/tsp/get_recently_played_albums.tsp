{
    "arguments":{
      "result-count": "6",
      "max-lastpltago": "604800"
    },
    "schemes":["album://"],
    "filterkeys": ["scheme","_pk_"],
    "filters":{
      "and":{
         "<=" :["object://lastpltago", "parameter://max-lastpltago"],
         ">=" :["object://plcnt", "value://1"],
         "!equals" :["object://deleted", "value://1"]
      }
    },
    "sorting":[
      ["descending-num", "lastplt"]
    ]
}
