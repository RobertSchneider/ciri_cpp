{
	"variables": 
	[
		{
			"name":"_give",
			"requirement": {
				"meanings":["ROOT"],
				"values":["give"]
			}
		},
		{
			"name":"_list",
			"requirement": {
				"meanings":["dobj"],
				"values":["list"],
				"depth_g":"_give"
			}
		},
		{
			"name":"type",
			"requirement": {
				"meanings":["pobj"],
				"values":["files"],
				"depth_g":"_list"
			}
		}
	],
	"actions":[
		{
			"type":"say",
			"value":"the $type are"
		},
		{
			"type":"sh",
			"value":"ls"
		}
	]
}