
extends Sprite

# member variables here, example:
# var a=2
# var b="textvar"

func _ready():
	set_process(true)
	pass


var wait=0.0
func _process(delta):
	wait += delta
	if (wait>=0.1):
		var rect = get_region_rect()
		rect.pos.x += 16
		if (rect.pos.x == 80 + 16*6):
			rect.pos.x -=16
			remove_and_delete_child(get_node("StaticBody2D"))
		wait -= 0.1
		set_region_rect(rect)

