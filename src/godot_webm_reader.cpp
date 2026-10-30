#include "godot_webm_reader.h"

webm::Status webm_gdextension::GodotWebmReader::Read(size_t num_to_read, uint8_t *buffer, uint64_t *num_actually_read) {
    if (file.is_null()) {
        *num_actually_read = 0;
        return webm::Status(webm::Status::kEndOfFile);
    }

    uint64_t actual = file->get_buffer(buffer, (uint64_t)num_to_read);
    *num_actually_read = actual;

    if (actual == 0) {
        return webm::Status(webm::Status::kEndOfFile);
    }

    if (actual == num_to_read) {
        return webm::Status(webm::Status::kOkCompleted);
    } else {
        return webm::Status(webm::Status::kOkPartial);
    }
}

webm::Status webm_gdextension::GodotWebmReader::Skip(uint64_t num_to_skip, uint64_t *num_actually_skipped) {
	if (file.is_null()) {
		*num_actually_skipped = 0;
		return webm::Status(webm::Status::kEndOfFile);
	}

	uint64_t previous_position = file->get_position();
	uint64_t length = file->get_length();
	if (previous_position >= length) {
		*num_actually_skipped = 0;
		return webm::Status(webm::Status::kEndOfFile);
	}

	uint64_t seek_position = previous_position + num_to_skip;
	if (seek_position > length) {
		seek_position = length;
	}
	file->seek(seek_position);

	uint64_t new_position = file->get_position();
	if (new_position < previous_position) {
		*num_actually_skipped = 0;
		return webm::Status(webm::Status::kEndOfFile);
	}

	uint64_t actual = new_position - previous_position;
	*num_actually_skipped = actual;

	if (actual == 0) {
		return webm::Status(webm::Status::kEndOfFile);
	}

	if (actual == num_to_skip) {
		return webm::Status(webm::Status::kOkCompleted);
	} else {
		return webm::Status(webm::Status::kOkPartial);
	}
}

uint64_t webm_gdextension::GodotWebmReader::Position() const {
	if (file.is_null()) {
		return 0;
	}

	return file->get_position();
}
