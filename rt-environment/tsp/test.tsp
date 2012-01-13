{
    "arguments":{
        "searchtext":"",
        "result-count":"250"
    },
    "schemes":["album://",
        "artist://","track://"
    ],
    "filters":{
        "or":{
              "contains":[
                          "object://name", "value://fire"
                         ],
              "and":{
                     "contains":["object://name","parameter://searchtext"],
                     "!contains":["object://name","value://ba"]
                    }
             }
    },
    "sorting":[
	["descending-num","object://year"],
	["ascending", "object://name"]
    ]
}
