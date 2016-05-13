
extends Spatial

# member variables here, example:
# var a=2
# var b="textvar"


func _ready():
	# Called every time the node is added to the scene.
	# Initialization here
	set_process(true)
	pass


func _process(delta):
	var hmd = get_node("Openvr").get_hmd_transform().scaled(Vector3(0.5, 0.5, 0.5))