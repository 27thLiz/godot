
extends Spatial

# member variables here, example:
# var a=2
# var b="textvar"
onready var vr = get_node("Openvr")
onready var cam_left = get_node("Left/Viewport/Camera")
onready var cam_right = get_node("Right/Viewport/Camera")
onready var inst_pos = get_node("../instance_pos")

func _ready():
	set_process(true)
	
func _process(delta):
	var hmd = vr.get_hmd_transform()
	cam_left.set_global_transform(hmd.translated(inst_pos.get_translation()).scaled(Vector3(0.1, 0.1, 0.1)))
	


