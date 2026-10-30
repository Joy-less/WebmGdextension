#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/resource_uid.hpp"
#include "godot_cpp/classes/resource_loader.hpp"

#include <opus/opus.h>

#include "resource_format_loader_webm.h"
#include "video_stream_webm.h"

PackedStringArray webm_gdextension::ResourceFormatLoaderWebm::_get_recognized_extensions() const {
	return { "webm" };
}

bool webm_gdextension::ResourceFormatLoaderWebm::_handles_type(const StringName &p_type) const {
	return p_type == SNAME_VideoStreamWebm || ClassDB::is_parent_class(p_type, SNAME_VideoStreamWebm);
}

String webm_gdextension::ResourceFormatLoaderWebm::_get_resource_type(const String &p_path) const {
	String extension = p_path.get_extension().to_lower();
	if (extension == "webm") {
		return "VideoStreamWebm";
	}
	return "";
}

Variant webm_gdextension::ResourceFormatLoaderWebm::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
	Ref<VideoStreamWebm> webm_stream;
	webm_stream.instantiate();

	webm_stream->set_file(p_path);

	return webm_stream;
}
