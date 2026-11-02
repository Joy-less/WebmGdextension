#pragma once

#include <demuxer/WebMDemuxer.h>
#include <decoder/VPXDecoder.h>
#include <decoder/OpusDecoder.h>
#include <decoder/VorbisDecoder.h>

#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/resource_format_loader.hpp"
#include "godot_cpp/classes/image_texture.hpp"
#include "godot_cpp/classes/video_stream.hpp"
#include "godot_cpp/classes/video_stream_playback.hpp"

#include "godot_webm_reader.h"

using namespace godot;

namespace webm_gdextension {

class VideoStreamPlaybackWebm : public VideoStreamPlayback {
	GDCLASS(VideoStreamPlaybackWebm, VideoStreamPlayback);

	String file_name;
	int audio_track = 0;

	WebMDemuxer *webm = nullptr;
	GodotWebmReader *webm_reader = nullptr;
	VPXDecoder *video = nullptr;
	OpusDecoder *audio_opus = nullptr;
	VorbisDecoder *audio_vorbis = nullptr;

	WebMFrame **video_frames = nullptr;
	WebMFrame *audio_frame = nullptr;
	int64_t video_frames_pos = 0;
	int64_t video_frames_capacity = 0;

	int num_decoded_samples = 0;
	int samples_offset = -1;

	bool playing = false;
	bool paused = false;
	bool seeking = false;
	double delay_compensation = 0.0;
	double time = 0.0;
	double video_frame_delay = 0.0;
	double video_pos = 0.0;

	PackedByteArray frame_data;
	Ref<ImageTexture> texture;

	PackedFloat32Array pcm;

protected:
	static void _bind_methods() {}

public:
	VideoStreamPlaybackWebm();
	~VideoStreamPlaybackWebm();

	bool set_file(const String &p_file);

	virtual void _stop() override;
	virtual void _play() override;

	virtual bool _is_playing() const override;

	virtual void _set_paused(bool p_paused) override;
	virtual bool _is_paused() const override;

	virtual double _get_length() const override;

	virtual double _get_playback_position() const override;
	virtual void _seek(double p_time) override;

	virtual void _set_audio_track(int p_idx) override;

	virtual Ref<Texture2D> _get_texture() const override;
	virtual void _update(double p_delta) override;

	virtual int _get_channels() const override;
	virtual int _get_mix_rate() const override;

private:
	inline bool has_enough_video_frames() const;
	bool should_process(const WebMFrame &video_frame);
	void delete_pointers();
};

/**/

class VideoStreamWebm : public VideoStream {
	GDCLASS(VideoStreamWebm, VideoStream);

protected:
	int audio_track = 0;

	static void _bind_methods();

public:
	virtual Ref<VideoStreamPlayback> _instantiate_playback() override {
		Ref<VideoStreamPlaybackWebm> pb = memnew(VideoStreamPlaybackWebm);
		pb->_set_audio_track(audio_track);
		pb->set_file(get_file());
		return pb;
	}

	virtual void set_audio_track(int p_track) { audio_track = p_track; }

	VideoStreamWebm() {}
};

};
