# singleton to load scenes

extends Node


var current_scene = null



func _ready():
	var root = get_scene().get_root()
	current_scene = root.get_child( root.get_child_count() -1 ) # last child of root always the loaded scene

func goto_scene(scene):
	var s = ResourceLoader.load(scene)
	current_scene.queue_free()
	current_scene = s.instance()
	get_scene().get_root().add_child(current_scene)