
extends KinematicBody2D

# member variables here, example:
# var a=2
# var b="textvar"

var red_trail_class = preload("data/4godot/trails.gd")
var explosion_class = preload("data/4godot/explosion.scn")
var rocket_class = preload("data/4godot/rocket.scn")
#var red_trail

func _ready():
	#red_trail = red_trail_class.new()
	#get_parent().add_child(red_trail)
	#red_trail.set_visible(true)
	#red_trail.set_tileset(get_parent().get_node("red").get_tileset())
	#set_pos(Vector2(64*16,64*16))
	set_fixed_process(true)
	speed_setting = get_parent().get_speed()
	pass



var wait = 0.2
var interp = 0.0
var dir = 0
var old_dir = 0
var speed_setting = 1
var turbo_modifier = 2
func _fixed_process(delta):
	if (is_in_game() != true):
		#print(":(")
		return
	#waiting because of movement in tilewise steps (too fast otherwise)
	interp += delta
	if (interp >= wait/speed_setting/turbo_modifier):
		movement()
		interp = 0


func set_dir(d):
	dir = d
	
	var rect = get_node("Sprite").get_region_rect()
	
	if (dir == 0):
		rect.pos.x=48
	if (dir == 1):
		rect.pos.x=64
	if (dir == 2):
		rect.pos.x=16
	if (dir == 3):
		rect.pos.x=32
	
	get_node("Sprite").set_region_rect(rect)
	


func movement():
	var pos = get_pos()
	var motion = Vector2(0,0)
	#var red = get_parent().get_node("red")
	#if (red extends preload("data/4godot/trails.gd")):
	#	red.add(pos.x/16,pos.y/16)
	
	if (dir == 0):
		motion.x += 16
	if (dir == 1):
		motion.y += 16
	if (dir == 2):
		motion.x -= 16
	if (dir == 3):
		motion.y -= 16
	
	pos = get_pos()
	move(Vector2(0,0))
	pos /= 16
	pos = pos.floor()
	pos *=16
	set_pos(pos)
	
	# handling collisions
	if (is_colliding()):
	#if (! can_move_to(motion+pos,true)):
		#print("D:")
		set_inactive()
	if (pos.x/16 > 32+get_parent().size-1 or pos.x/16 < 32 or pos.y/16 > 32+get_parent().size-1 or pos.y/16 < 32):
		set_inactive()
		get_child("Sprite").hide()
	pos = get_pos()
	
	if (is_in_game()):
		set_pos(motion+pos)
	pos = get_pos()
	move(Vector2(0,0))
	pos /= 16
	pos = pos.floor()
	pos *=16
	set_pos(pos)


var in_game = true
func is_in_game():
	return in_game

func set_inactive():
	in_game = false
	
	#set sprite to destroyed tile
	var rect = get_node("Sprite").get_region_rect()
	rect.pos.y = 16
	rect.pos.x = 192
	get_node("Sprite").set_region_rect(rect)
	
	#destruction
	var pos = get_pos()
	var red = get_parent().get_node("red")
	if (red extends preload("data/4godot/trails.gd")):
		red.remove(pos.x/16,pos.y/16)
		red.remove((pos.x+16)/16,pos.y/16)
		red.remove((pos.x-16)/16,pos.y/16)
		red.remove(pos.x/16,(pos.y+16)/16)
		red.remove(pos.x/16,(pos.y-16)/16)
		red = get_parent().get_node("blue")
	if (red extends preload("data/4godot/trails.gd")):
		red.remove(pos.x/16,pos.y/16)
		red.remove((pos.x+16)/16,pos.y/16)
		red.remove((pos.x-16)/16,pos.y/16)
		red.remove(pos.x/16,(pos.y+16)/16)
		red.remove(pos.x/16,(pos.y-16)/16)
		red = get_parent().get_node("yellow")
	if (red extends preload("data/4godot/trails.gd")):
		red.remove(pos.x/16,pos.y/16)
		red.remove((pos.x+16)/16,pos.y/16)
		red.remove((pos.x-16)/16,pos.y/16)
		red.remove(pos.x/16,(pos.y+16)/16)
		red.remove(pos.x/16,(pos.y-16)/16)
		red = get_parent().get_node("green")
	if (red extends preload("data/4godot/trails.gd")):
		red.remove(pos.x/16,pos.y/16)
		red.remove((pos.x+16)/16,pos.y/16)
		red.remove((pos.x-16)/16,pos.y/16)
		red.remove(pos.x/16,(pos.y+16)/16)
		red.remove(pos.x/16,(pos.y-16)/16)
	
	#explosions
	var e1 = explosion_class.instance()
	var e2 = explosion_class.instance()
	var e3 = explosion_class.instance()
	var e4 = explosion_class.instance()
	var e5 = explosion_class.instance()
	e1.set_pos(Vector2(pos.x+16,pos.y))
	e2.set_pos(Vector2(pos.x-16,pos.y))
	e3.set_pos(Vector2(pos.x,pos.y+16))
	e4.set_pos(Vector2(pos.x,pos.y-16))
	e5.set_pos(Vector2(pos.x,pos.y))
	get_parent().get_child("explosion_container").add_child(e1)
	get_parent().get_child("explosion_container").add_child(e2)
	get_parent().get_child("explosion_container").add_child(e3)
	get_parent().get_child("explosion_container").add_child(e4)
	get_parent().get_child("explosion_container").add_child(e5)
	#e1._ready()
	
	
	#get_parent().remove_child(get_parent().get_child(get_index()))
	
	queue_free()
	
	#pos.x += 16*3
	#set_pos(pos)
	#move(Vector2(0,0))
	#in_game = true
	

