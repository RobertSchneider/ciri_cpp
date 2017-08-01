{
	"variables": 
	[
		{
			"name":"root",
			"requirement": {
				"meaning":"ROOT",
				"values":["create"],
				"depth":0
			}
		},
		{
			"name":"type",
			"requirement": {
				"meaning":"dobj",
				"values":["file"],
				"depth":1
			}
		},
		{
			"name":"_called",
			"requirement": {
				"meaning":"partmod",
				"values":["called", "named"]
			}
		},
		
		{
			"name":"name",
			"requirement": {
				"meaning":"dep",
				"depth_g":"_called"
			}
		}
	],
	"actions":[
		{
			"type":"say",
			"value":"creating $type"
		},
		{
			"type":"sh",
			"value":"touch $name"
		}
	]
}