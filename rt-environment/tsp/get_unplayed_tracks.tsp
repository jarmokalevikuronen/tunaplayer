{
    "arguments":{
        "result-count":"20000"
    },
    "schemes":["track://"],
    "filterkeys": ["_pk_", "scheme", "fn", "crtt", "year"]
    "filters":{
        "or":{
              "<":[
                          "object://play-count", "value://1"
                        ]
             }
    },
    "sorting":[
	["ascending-num","object://name"]
    ]
}
