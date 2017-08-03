{
	"variables": 
	[
		{
			"name":"_create",
			"requirement": {
				"meanings":["ROOT"],
				"values":["create", "add", "make"],
				"depth":0
			}
		},
		{
			"name":"type",
			"requirement": {
				"meanings":["dobj"],
				"values":["file", "directory", "folder"],
				"depth_g":"_create"
			}
		},
		{
			"name":"_called",
			"requirement": {
				"meanings":["acl"],
				"values":["called", "named"]
			}
		},
		
		{
			"name":"name",
			"requirement": {
				"meanings":["oprd", "dobj"],
				"depth_g":"_called"
			}
		}
	],
	"actions":[
		{
			"type":"say",
			"value":"creating $type $_called $name"
		},
		{
			"type":"sh",
			"conditions": [
				["type","file"]
			],
			"value":"touch $name"
		},
		{
			"type":"sh",
			"conditions": [
				["type","directory", "folder"]
			],
			"value":"mkdir $name"
		}
	]
}