{
    "arguments":{
        "result-count":"20000"
    },
    "schemes":["track://"],
    "filters":{
        "or":{
              "exists":[
                          "object://lastplayed", "value://0"
                        ]
             }
    },
    "sorting":[
	["ascending-num","object://name"]
    ]
}
