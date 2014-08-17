
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
	pass



var wait = 0.1
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
		interp -= wait/speed_setting/turbo_modifier


func set_dir(d):
	dir = d
	old_dir = dir


func movement():
	var pos = get_pos()
	var motion = Vector2(0,0)
	#var red = get_parent().get_node("red")
	#if (red extends preload("data/4godot/trails.gd")):
	#	red.add(pos.x/16,pos.y/16)
	
	#make sure no one turns accidentally in the opposite direction
	if (dir == 0 && old_dir == 2):
		dir = old_dir
	if (dir == 1 && old_dir == 3):
		dir = old_dir
	if (dir == 2 && old_dir == 0):
		dir = old_dir
	if (dir == 3 && old_dir == 1):
		dir = old_dir
	
	
	var rect = get_node("Sprite").get_region_rect()
	
	if (dir == 0):
		motion.x += 16
		rect.pos.x=48
	if (dir == 1):
		motion.y += 16
		rect.pos.x=64
	if (dir == 2):
		motion.x -= 16
		rect.pos.x=16
	if (dir == 3):
		motion.y -= 16
		rect.pos.x=32
	
	get_node("Sprite").set_region_rect(rect)
	
	old_dir = dir
	
	pos = get_pos()
	move(Vector2(0,0))
	pos /= 16
	pos = pos.floor()
	pos *=16
	set_pos(pos)
	
	# handling collisions
	if (is_colliding()):
	#if (! can_move_to(motion+pos,true)):
		print("D:")
		set_inactive()
	
	pos = get_pos()
	
	if (is_in_game()):
		set_pos(motion+pos)
	#if (is_in_game()):
	#	pos = get_pos()
	#	move(Vector2(0,0))
	#	pos /= 16
	#	pos = pos.floor()
	#	pos *=16
	#	set_pos(pos)
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
	
	#explosions
	var e1 = explosion_class.instance()
	var e2 = explosion_class.instance()
	var e3 = explosion_class.instance()
	var e4 = explosion_class.instance()
	e1.set_pos(Vector2(pos.x+16,pos.y))
	e2.set_pos(Vector2(pos.x-16,pos.y))
	e3.set_pos(Vector2(pos.x,pos.y+16))
	e4.set_pos(Vector2(pos.x,pos.y-16))
	get_parent().get_child("explosion_container").add_child(e1)
	get_parent().get_child("explosion_container").add_child(e2)
	get_parent().get_child("explosion_container").add_child(e3)
	get_parent().get_child("explosion_container").add_child(e4)
	#e1._ready()
	
	
	get_parent().remove_child(get_parent().get_child(get_index()))
	
	#pos.x += 16*3
	#set_pos(pos)
	#move(Vector2(0,0))
	#in_game = true
	

