#pragma once

#include <demuxer/WebMDemuxer.h>

#include "godot_cpp/classes/file_access.hpp"

using namespace godot;

namespace webm_gdextension {

// This class is an alternative to webm::FileReader that uses Godot's FileAccess.
class GodotWebmReader : public webm::Reader {
public:
	GodotWebmReader(const String &p_file) {
		file = FileAccess::open(p_file, FileAccess::READ);

		ERR_FAIL_COND_MSG(file.is_null(), "Failed loading resource: '" + p_file + "'.");
	}
	~GodotWebmReader() {}

	virtual webm::Status Read(size_t num_to_read, uint8_t *buffer, uint64_t *num_actually_read) override;
	virtual webm::Status Skip(uint64_t num_to_skip, uint64_t *num_actually_skipped) override;
	virtual uint64_t Position() const override;

private:
	Ref<FileAccess> file;
};

};
