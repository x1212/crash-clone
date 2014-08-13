
extends TileMap

# member variables here, example:
# var a=2
# var b="textvar"

func _ready():
	# Initalization here
	pass

func add(x,y):
	set_cell(x,y,0)

func remove(x,y):
	set_cell(x,y,-1)