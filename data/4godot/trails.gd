
extends TileMap

# member variables here, example:
# var a=2
# var b="textvar"

func _ready():
	# Initalization here
	pass

func add(x,y):

	set_cell(x,y,get_correct_tile(x,y))
	
	if (get_cell(x+1,y)!=-1):
		set_cell(x+1,y,get_correct_tile(x+1,y))
	if (get_cell(x-1,y)!=-1):
		set_cell(x-1,y,get_correct_tile(x-1,y))
	if (get_cell(x,y+1)!=-1):
		set_cell(x,y+1,get_correct_tile(x,y+1))
	if (get_cell(x,y-1)!=-1):
		set_cell(x,y-1,get_correct_tile(x,y-1))

func remove(x,y):
	set_cell(x,y,-1)
	
	if (get_cell(x+1,y)!=-1):
		set_cell(x+1,y,get_correct_tile(x+1,y))
	if (get_cell(x-1,y)!=-1):
		set_cell(x-1,y,get_correct_tile(x-1,y))
	if (get_cell(x,y+1)!=-1):
		set_cell(x,y+1,get_correct_tile(x,y+1))
	if (get_cell(x,y-1)!=-1):
		set_cell(x,y-1,get_correct_tile(x,y-1))

func get_correct_tile(x,y):
	var offset = 0
	
	
	if (get_cell(x+1,y) != -1):
		offset += 1
	if (get_cell(x,y+1) != -1):
		offset += 2
	if (get_cell(x,y-1) != -1):
		offset += 4
	if (get_cell(x-1,y) != -1):
		offset += 8
	
	return offset