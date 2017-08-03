{
	"variables": 
	[
		{
			"name":"_tell",
			"requirement": {
				"meanings":["ROOT"],
				"values":["tell"]
			}
		},
		{
			"name":"_time",
			"requirement": {
				"meanings":["dobj"],
				"values":["time"],
				"depth_g":"_tell"
			}
		}
	],
	"actions":[
		{
			"type":"say",
			"value":"the $_time is"
		},
		{
			"type":"sh",
			"value":"date"
		},
		{
			"type":"commit",
			"value":""
		}
	]
}