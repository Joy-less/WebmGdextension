extends Node

func _ready() -> void:
	var stream1: VideoStreamWebm = load("res://samples/godot_engine_2025_showreel_480p.webm")
	print(stream1.file)
	
	$VideoStreamPlayer.stream = stream1
	$VideoStreamPlayer.play()
	
	#await get_tree().create_timer(1.0).timeout
	#$VideoStreamPlayer.stream_position = 100.0
