{
	"variables": 
	[
		{
			"name":"_execute",
			"requirement": {
				"meanings":["ROOT"],
				"values":["execute", "run"],
				"depth":0
			}
		},
		{
			"name":"cmd",
			"requirement": {
				"meanings":["dobj"],
				"values":["command", "bash", "shell"],
				"depth_g":"_execute"
			}
		}
	],
	"actions":[
		{
			"type":"say",
			"value":"creating $type $_called $name"
		}
	]
}