extends Node

func _ready() -> void:
	var stream1: VideoStreamWebm = load("res://samples/HDR10+_Sample_Video gbrp10le.webm")
	print(stream1.file)
	
	$VideoStreamPlayer.stream = stream1
	$VideoStreamPlayer.play()
	
	#await get_tree().create_timer(1.0).timeout
	#$VideoStreamPlayer.stream_position = 100.0
