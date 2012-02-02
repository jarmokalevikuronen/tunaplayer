{
    "arguments":{
      "result-count": "6",
      "max-age": "604800"
    },
    "schemes":["album://"],
    "filterkeys": ["scheme","_pk_"],
    "filters":{
      "and":{
         "!equals" :["object://deleted", "value://1"],
         "<=" :["object://age", "parameter://max-age"]
      }
    },
    "sorting":[
      ["ascending-num", "age"]
    ]
}
