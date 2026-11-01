#include "register_types.h"

#include "gdextension_interface.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/defs.hpp"
#include "godot_cpp/godot.hpp"
#include "godot_cpp/classes/resource_loader.hpp"

#include "video_stream_webm.h"
#include "resource_format_loader_webm.h"

using namespace godot;
using namespace webm_gdextension;

static Ref<ResourceFormatLoaderWebm> resource_format_loader_webm;

void initialize_webm_gdextension_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(VideoStreamPlaybackWebm);
		GDREGISTER_CLASS(VideoStreamWebm);
		GDREGISTER_CLASS(ResourceFormatLoaderWebm);

		resource_format_loader_webm.instantiate();
		ResourceLoader::get_singleton()->add_resource_format_loader(resource_format_loader_webm, true);
	}
}

void uninitialize_webm_gdextension_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		ResourceLoader::get_singleton()->remove_resource_format_loader(resource_format_loader_webm);
		resource_format_loader_webm.unref();
	}
}

extern "C" {
// Initialization
GDExtensionBool GDE_EXPORT webm_gdextension_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	init_obj.register_initializer(initialize_webm_gdextension_module);
	init_obj.register_terminator(uninitialize_webm_gdextension_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}
