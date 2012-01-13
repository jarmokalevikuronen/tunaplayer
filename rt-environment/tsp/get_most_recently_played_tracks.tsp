{
    "arguments":{
        "result-count":"50"
    },
    "schemes":["track://"],
    "filters":{
        "or":{
              "greaterthan":[
                          "object://lastplayed", "value://0"
                        ]
             }
    },
    "sorting":[
	["ascending-num","object://lastplayed"]
    ]
}
