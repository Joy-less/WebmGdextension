#include "video_stream_webm.h"

#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/os.hpp"
#include "godot_cpp/classes/audio_server.hpp"

#include <yuv2rgb.h>

// Disable shadowed declaration warnings for libwebm's status.h.
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4458)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif

// libsimplewebm
#include <decoder/OpusDecoder.h>
#include <decoder/VPXDecoder.h>
#include <decoder/VorbisDecoder.h>
#include <webm/status.h>

// Restore shadowed declaration warnings.
#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

// libvpx
#include <vpx/vpx_image.h>

webm_gdextension::VideoStreamPlaybackWebm::VideoStreamPlaybackWebm() {
	texture.instantiate();
}
webm_gdextension::VideoStreamPlaybackWebm::~VideoStreamPlaybackWebm() {
	delete_pointers();
}

bool webm_gdextension::VideoStreamPlaybackWebm::set_file(const String &p_file) {
	file_name = p_file;

	webm_reader = memnew(GodotWebmReader(file_name));
	webm = memnew(WebMDemuxer(webm_reader, 0, audio_track));
	if (!webm->isOpen()) {
		delete_pointers();
		return false;
	}

	video = memnew(VPXDecoder(*webm, OS::get_singleton()->get_processor_count()));
	if (!video->isOpen()) {
		delete_pointers();
		return false;
	}

	switch (webm->getAudioCodec()) {
		case WebMDemuxer::AUDIO_OPUS: {
			audio_opus = memnew(OpusDecoder(*webm));
			if (audio_opus->isOpen()) {
				audio_frame = memnew(WebMFrame);
				pcm = PackedFloat32Array();
				pcm.resize(sizeof(float) * audio_opus->getBufferSamples() * webm->getChannels());
			} else {
				memdelete(audio_opus);
				audio_opus = nullptr;
			}
		} break;
		case WebMDemuxer::AUDIO_VORBIS: {
			audio_vorbis = memnew(VorbisDecoder(*webm));
			if (audio_vorbis->isOpen()) {
				audio_frame = memnew(WebMFrame);
				pcm = PackedFloat32Array();
				pcm.resize(sizeof(float) * audio_vorbis->getBufferSamples() * webm->getChannels());
			} else {
				memdelete(audio_vorbis);
				audio_vorbis = nullptr;
			}
		} break;
		case WebMDemuxer::NO_AUDIO:
		default: {
		} break;
	}

	frame_data.resize((webm->getWidth() * webm->getHeight()) << 2);
	Ref<Image> img = Image::create_empty(webm->getWidth(), webm->getHeight(), false, Image::FORMAT_RGBA8);
	texture->set_image(img);

	return true;
}

void webm_gdextension::VideoStreamPlaybackWebm::_stop() {
	if (playing) {
		delete_pointers();

		set_file(file_name); // Should not fail here...

		video_frames_capacity = 0;
		video_frames_pos = 0;
		num_decoded_samples = 0;
		samples_offset = -1;
		video_frame_delay = video_pos = 0.0;
	}
	time = 0.0;
	seeking = false;
	playing = false;
}
void webm_gdextension::VideoStreamPlaybackWebm::_play() {
	_stop();

	delay_compensation = ProjectSettings::get_singleton()->get("audio/video_delay_compensation_ms");
	delay_compensation /= 1000.0;

	playing = true;
}

bool webm_gdextension::VideoStreamPlaybackWebm::_is_playing() const {
	return playing;
}

void webm_gdextension::VideoStreamPlaybackWebm::_set_paused(bool p_paused) {
	paused = p_paused;
}
bool webm_gdextension::VideoStreamPlaybackWebm::_is_paused() const {
	return paused;
}

double webm_gdextension::VideoStreamPlaybackWebm::_get_length() const {
	if (webm) {
		return webm->getLength();
	}
	return 0.0f;
}

double webm_gdextension::VideoStreamPlaybackWebm::_get_playback_position() const {
	return video_pos;
}
void webm_gdextension::VideoStreamPlaybackWebm::_seek(double p_time) {
	if (!webm) {
		return;
	}
	if (p_time >= webm->getLength()) {
		return;
	}

	if (webm->seek(p_time) < 0.0) {
		return;
	}

	if (video) {
		video->reset();
	}
	if (audio_opus) {
		audio_opus->reset();
	}
	if (audio_vorbis) {
		audio_vorbis->reset();
	}

	video_frames_pos = 0;
	num_decoded_samples = 0;
	samples_offset = -1;

	seeking = true;
	time = p_time;
	video_pos = p_time;
}

void webm_gdextension::VideoStreamPlaybackWebm::_set_audio_track(int p_idx) {
	audio_track = p_idx;
}

Ref<Texture2D> webm_gdextension::VideoStreamPlaybackWebm::_get_texture() const {
	return texture;
}

void webm_gdextension::VideoStreamPlaybackWebm::_update(double p_delta) {
	if ((!playing || paused) || !video) {
		return;
	}

	if (!seeking) {
		time += p_delta;

		if (time < video_pos) {
			return;
		}
	}

	bool audio_buffer_full = false;
	bool reached_eos = false;

	if (samples_offset > -1) {
		// Mix remaining samples
		const int to_read = num_decoded_samples - samples_offset;
		const int mixed = mix_audio(to_read, pcm, (samples_offset * webm->getChannels()));
		if (mixed != to_read) {
			samples_offset += mixed;
			audio_buffer_full = true;
		} else {
			samples_offset = -1;
		}
	}

	const bool has_audio = (audio_opus || audio_vorbis);
	while ((has_audio && !audio_buffer_full && !has_enough_video_frames()) || (!has_audio && video_frames_pos == 0)) {
		if (has_audio && !audio_buffer_full && audio_frame->isValid()) {
			bool get_pcmf_succeeded;
			if (audio_opus) {
				get_pcmf_succeeded = audio_opus->getPCMF(*audio_frame, pcm.ptrw(), pcm.size(), num_decoded_samples);
			} else {
				get_pcmf_succeeded = audio_vorbis->getPCMF(*audio_frame, pcm.ptrw(), pcm.size(), num_decoded_samples);
			}
			if (get_pcmf_succeeded && num_decoded_samples > 0) {
				if (!seeking) {
					const int mixed = mix_audio(num_decoded_samples, pcm);

					if (mixed != num_decoded_samples) {
						samples_offset = mixed;
						audio_buffer_full = true;
					}
				}
			}
		}

		if (video_frames_pos >= video_frames_capacity) {
			WebMFrame **video_frames_new = (WebMFrame **)memrealloc(video_frames, ++video_frames_capacity * sizeof(void *));
			ERR_FAIL_COND(!video_frames_new); // Out of memory
			(video_frames = video_frames_new)[video_frames_capacity - 1] = memnew(WebMFrame);
		}
		WebMFrame *video_frame = video_frames[video_frames_pos];

		if (!webm->readFrame(video_frame, audio_frame)) {
			reached_eos = true;
			break;
		}

		if (video_frame->isValid()) {
			++video_frames_pos;
		}
	}

	bool video_frame_done = false;
	while (video_frames_pos > 0 && !video_frame_done) {
		WebMFrame *video_frame = video_frames[0];

		// It seems VPXDecoder::decode has to be executed even though we might skip this frame
		if (video->decode(*video_frame)) {
			VPXDecoder::IMAGE_ERROR err;
			VPXDecoder::Image image;

			if (should_process(*video_frame)) {
				if ((err = video->getImage(image)) != VPXDecoder::NO_FRAME) {
					if (err == VPXDecoder::NO_ERROR && image.w == webm->getWidth() && image.h == webm->getHeight()) {
						bool converted = false;

						if (image.chromaShiftW == 0 && image.chromaShiftH == 0 && image.cs == VPX_CS_SRGB) { // sRGB format.
							uint8_t *wp = frame_data.ptrw();
							const unsigned char *rRow = image.planes[2];
							const unsigned char *gRow = image.planes[0];
							const unsigned char *bRow = image.planes[1];
							const unsigned char *aRow = image.alpha;
							for (int y = 0; y < image.h; y++) {
								for (int x = 0; x < image.w; x++) {
									*wp++ = rRow[x];
									*wp++ = gRow[x];
									*wp++ = bRow[x];
									*wp++ = aRow ? aRow[x] : 255;
								}
								rRow += image.linesize[2];
								gRow += image.linesize[0];
								bRow += image.linesize[1];
								if (aRow) {
									aRow += image.alphaLinesize;
								}
							}
							converted = true;
						} else { // YUV format.
							if (image.chromaShiftW == 1 && image.chromaShiftH == 1) { // YUV 420 format.
								uint8_t *wp = frame_data.ptrw();
								yuv420_2_rgb8888(wp, image.planes[0], image.planes[1], image.planes[2], image.w, image.h, image.linesize[0], image.linesize[1], image.w << 2);
								converted = true;
							} else if (image.chromaShiftW == 1 && image.chromaShiftH == 0) { // YUV 422 format.
								uint8_t *wp = frame_data.ptrw();
								yuv422_2_rgb8888(wp, image.planes[0], image.planes[1], image.planes[2], image.w, image.h, image.linesize[0], image.linesize[1], image.w << 2);
								converted = true;
							} else if (image.chromaShiftW == 0 && image.chromaShiftH == 0) { // YUV 444 format.
								uint8_t *wp = frame_data.ptrw();
								yuv444_2_rgb8888(wp, image.planes[0], image.planes[1], image.planes[2], image.w, image.h, image.linesize[0], image.linesize[1], image.w << 2);
								converted = true;
							} else if (image.chromaShiftW == 2 && image.chromaShiftH == 0) { // YUV 411 format.
								// This format is rare and thus unsupported by yuv2rgb.h, so discard the frame.
							}

							if (converted && image.alpha) {
								uint8_t *wp = frame_data.ptrw();
								const unsigned char *aRow = image.alpha;
								for (int y = 0; y < image.h; y++) {
									for (int x = 0; x < image.w; x++) {
										wp += 3;
										*wp++ = aRow[x];
									}
									aRow += image.alphaLinesize;
								}
							}
						}

						if (converted) {
							Ref<Image> img = Image::create_from_data(image.w, image.h, false, Image::FORMAT_RGBA8, frame_data);
							texture->set_image(img); // Zero copy send to visual server
							video_frame_done = true;

							if (seeking) {
								seeking = false;
							}
						}
					}
				}
			}
		}

		if (!seeking) {
			video_pos = video_frame->time;
		}
		memmove(video_frames, video_frames + 1, (--video_frames_pos) * sizeof(void *));
		video_frames[video_frames_pos] = video_frame;
	}

	if (video_frames_pos == 0 && reached_eos) {
		_stop();
	}
}

int webm_gdextension::VideoStreamPlaybackWebm::_get_channels() const {
	if (audio_opus || audio_vorbis) {
		return webm->getChannels();
	}
	return 0;
}
int webm_gdextension::VideoStreamPlaybackWebm::_get_mix_rate() const {
	if (audio_opus || audio_vorbis) {
		return webm->getSampleRate();
	}
	return 0;
}

inline bool webm_gdextension::VideoStreamPlaybackWebm::has_enough_video_frames() const {
	if (video_frames_pos > 0) {
		// FIXME: AudioServer output latency was fixed in af9bb0e, previously it used to
		// systematically return 0. Now that it gives a proper latency, it broke this
		// code where the delay compensation likely never really worked.
		//const double audio_delay = AudioServer::get_singleton()->get_output_latency();
		const double video_time = video_frames[video_frames_pos - 1]->time;
		return video_time >= time + /* audio_delay + */ (seeking ? 0.0 : delay_compensation);
	}
	return false;
}

bool webm_gdextension::VideoStreamPlaybackWebm::should_process(const WebMFrame &video_frame) {
	// FIXME: AudioServer output latency was fixed in af9bb0e, previously it used to
	// systematically return 0. Now that it gives a proper latency, it broke this
	// code where the delay compensation likely never really worked.
	//const double audio_delay = AudioServer::get_singleton()->get_output_latency();
	return video_frame.time >= time + /* audio_delay + */ (seeking ? 0.0 : delay_compensation);
}

void webm_gdextension::VideoStreamPlaybackWebm::delete_pointers() {
	pcm = PackedFloat32Array();

	if (audio_frame) {
		memdelete(audio_frame);
		audio_frame = nullptr;
	}
	if (video_frames) {
		for (int i = 0; i < video_frames_capacity; ++i) {
			memdelete(video_frames[i]);
		}
		memfree(video_frames);
		video_frames = nullptr;
	}

	if (video) {
		memdelete(video);
		video = nullptr;
	}
	if (audio_opus) {
		memdelete(audio_opus);
		audio_opus = nullptr;
	}
	if (audio_vorbis) {
		memdelete(audio_vorbis);
		audio_vorbis = nullptr;
	}
	if (webm) {
		memdelete(webm);
		webm = nullptr;
	}
	if (webm_reader) {
		memdelete(webm_reader);
		webm_reader = nullptr;
	}
}

/**/

void webm_gdextension::VideoStreamWebm::_bind_methods() {}
