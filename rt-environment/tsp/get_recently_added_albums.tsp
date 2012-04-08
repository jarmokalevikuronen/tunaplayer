{
    "arguments":{
      "result-count": "6",
      "max-age": "604800",
      "userprofile": "/"
    },
    "schemes":["album://"],
    "filterkeys": ["scheme","_pk_"],
    "filters":{
      "and":{
         "!equals" :["object://deleted", "value://1"],
         "<=" :["object://age", "parameter://max-age"],
         "contains":["object://_d.usr", "parameter://userprofile"]
      }
    },
    "sorting":[
      ["ascending-num", "age"]
    ]
}
