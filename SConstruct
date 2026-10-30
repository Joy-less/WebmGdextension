#!/usr/bin/env python
import os
import sys

from methods import print_error


libname = "WebmGdextension"
projectdir = "project"

localEnv = Environment(tools=["default"], PLATFORM="")

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"api_version": 4.5, "env": env, "customs": customs})

# Set standard requirements (see https://github.com/godotengine/godot/blob/master/SConstruct#L915)
# These are added automatically by Godot, but since this is a GDExtension, we need to add them manually
if env.get("is_msvc", False):
    env.Prepend(CFLAGS=["/std:c17"])
    env.Prepend(CXXFLAGS=["/std:c++17"])
    env.Prepend(CCFLAGS=["/permissive-"])
    env.Prepend(CXXFLAGS=["/Zc:__cplusplus"])
else:
    env.Prepend(CFLAGS=["-std=gnu17"])
    env.Prepend(CXXFLAGS=["-std=gnu++17"])

# LIBOPUS: Add WIN32 define on Windows (required by config.h)
if env["platform"] == "windows":
    env.Append(CPPDEFINES=["WIN32"])

env.Append(CPPDEFINES=[
    # LIBOPUS DEFINES:
    "HAVE_CONFIG_H",

    # LIBSIMPLEWEBM DEFINES:
    "LIBSIMPLEWEBM_DISABLE_VP8",
])

env.Append(CPPPATH=[
    "src/",

    # LIBOPUS INCLUDES:
    "#thirdparty/libopus/",
    "#thirdparty/libopus/opus/",
    "#thirdparty/libopus/src/",
    "#thirdparty/libopus/celt/",
    "#thirdparty/libopus/silk/",
    "#thirdparty/libopus/silk/fixed/",
    "#thirdparty/libopus/silk/float/",
    "#thirdparty/libopus/celt/x86/",
    "#thirdparty/libopus/silk/x86/",
    "#thirdparty/libopus/silk/fixed/x86/",
    "#thirdparty/libopus/silk/float/x86/",
    "#thirdparty/libopus/celt/arm/",
    "#thirdparty/libopus/silk/arm/",
    "#thirdparty/libopus/silk/fixed/arm/",

    # LIBOGG INCLUDES:
    "#thirdparty/libogg/",

    # LIBVORBIS INCLUDES:
    "#thirdparty/libvorbis/",

    # LIBSIMPLEWEBM INCLUDES:
    "#thirdparty/libsimplewebm/",
    "#thirdparty/libsimplewebm/include/",
    "#thirdparty/libsimplewebm/libwebm/webm_parser",
    "#thirdparty/libsimplewebm/libwebm/webm_parser/include",

    # LIBVPX INCLUDES:
    "#thirdparty/libvpx/",

    # MISC INCLUDES:
    "#thirdparty/misc/",
])

sources = Glob("src/*.cpp")

# LIBOPUS SOURCES:
sources = (sources
    + Glob("#thirdparty/libopus/src/*.c")
    + Glob("#thirdparty/libopus/celt/*.c")
    + Glob("#thirdparty/libopus/silk/*.c")
    + Glob("#thirdparty/libopus/silk/float/*.c")
)

# LIBOGG SOURCES:
sources = (sources
    + Glob("#thirdparty/libogg/bitwise.c")
    + Glob("#thirdparty/libogg/framing.c")
)

# LIBVORBIS SOURCES:
sources = (sources
    # + Glob("#thirdparty/libvorbis/barkmel.c")
    + Glob("#thirdparty/libvorbis/bitrate.c")
    + Glob("#thirdparty/libvorbis/block.c")
    + Glob("#thirdparty/libvorbis/codebook.c")
    + Glob("#thirdparty/libvorbis/envelope.c")
    + Glob("#thirdparty/libvorbis/floor0.c")
    + Glob("#thirdparty/libvorbis/floor1.c")
    + Glob("#thirdparty/libvorbis/info.c")
    + Glob("#thirdparty/libvorbis/lookup.c")
    + Glob("#thirdparty/libvorbis/lpc.c")
    + Glob("#thirdparty/libvorbis/lsp.c")
    + Glob("#thirdparty/libvorbis/mapping0.c")
    + Glob("#thirdparty/libvorbis/mdct.c")
    + Glob("#thirdparty/libvorbis/psy.c")
    # + Glob("#thirdparty/libvorbis/psytune.c")
    + Glob("#thirdparty/libvorbis/registry.c")
    + Glob("#thirdparty/libvorbis/res0.c")
    + Glob("#thirdparty/libvorbis/sharedbook.c")
    + Glob("#thirdparty/libvorbis/smallft.c")
    + Glob("#thirdparty/libvorbis/synthesis.c")
    # + Glob("#thirdparty/libvorbis/tone.c")
    + Glob("#thirdparty/libvorbis/vorbisfile.c")
    + Glob("#thirdparty/libvorbis/window.c")
)

# LIBSIMPLEWEBM SOURCES:
sources = (sources
    + Glob("#thirdparty/libsimplewebm/decoder/OpusDecoder.cpp")
    + Glob("#thirdparty/libsimplewebm/decoder/VorbisDecoder.cpp")
    + Glob("#thirdparty/libsimplewebm/decoder/VPXDecoder.cpp")
    + Glob("#thirdparty/libsimplewebm/demuxer/WebMDemuxer.cpp")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/ancestory.cc")  # codespell:ignore ancestory
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/bit_utils.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/block_header_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/block_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/buffer_reader.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/callback.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/date_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/file_reader.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/float_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/id_element_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/id_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/istream_reader.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/master_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/parser_utils.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/segment_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/size_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/skip_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/unknown_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/var_int_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/virtual_block_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/void_parser.cc")
    + Glob("#thirdparty/libsimplewebm/libwebm/webm_parser/src/webm_parser.cc")
)

# LIBVPX SOURCES:
sources = (sources
    # + Glob("#thirdparty/libvpx/vp8/vp8_dx_iface.c")
    # + Glob("#thirdparty/libvpx/vp8/common/generic/systemdependent.c")
    # + Glob("#thirdparty/libvpx/vp8/common/alloccommon.c")
    # + Glob("#thirdparty/libvpx/vp8/common/blockd.c")
    # + Glob("#thirdparty/libvpx/vp8/common/copy_c.c")
    # + Glob("#thirdparty/libvpx/vp8/common/debugmodes.c")
    # + Glob("#thirdparty/libvpx/vp8/common/dequantize.c")
    # + Glob("#thirdparty/libvpx/vp8/common/entropy.c")
    # + Glob("#thirdparty/libvpx/vp8/common/entropymode.c")
    # + Glob("#thirdparty/libvpx/vp8/common/entropymv.c")
    # + Glob("#thirdparty/libvpx/vp8/common/extend.c")
    # + Glob("#thirdparty/libvpx/vp8/common/filter.c")
    # + Glob("#thirdparty/libvpx/vp8/common/findnearmv.c")
    # + Glob("#thirdparty/libvpx/vp8/common/idct_blk.c")
    # + Glob("#thirdparty/libvpx/vp8/common/idctllm.c")
    # + Glob("#thirdparty/libvpx/vp8/common/loopfilter_filters.c")
    # + Glob("#thirdparty/libvpx/vp8/common/mbpitch.c")
    # + Glob("#thirdparty/libvpx/vp8/common/modecont.c")
    # + Glob("#thirdparty/libvpx/vp8/common/quant_common.c")
    # + Glob("#thirdparty/libvpx/vp8/common/reconinter.c")
    # + Glob("#thirdparty/libvpx/vp8/common/reconintra.c")
    # + Glob("#thirdparty/libvpx/vp8/common/reconintra4x4.c")
    # + Glob("#thirdparty/libvpx/vp8/common/rtcd.c")
    # + Glob("#thirdparty/libvpx/vp8/common/setupintrarecon.c")
    # + Glob("#thirdparty/libvpx/vp8/common/swapyv12buffer.c")
    # + Glob("#thirdparty/libvpx/vp8/common/treecoder.c")
    # + Glob("#thirdparty/libvpx/vp8/common/vp8_loopfilter.c")
    # + Glob("#thirdparty/libvpx/vp8/decoder/dboolhuff.c")
    # + Glob("#thirdparty/libvpx/vp8/decoder/decodeframe.c")
    # + Glob("#thirdparty/libvpx/vp8/decoder/decodemv.c")
    # + Glob("#thirdparty/libvpx/vp8/decoder/detokenize.c")
    # + Glob("#thirdparty/libvpx/vp8/decoder/onyxd_if.c")
    + Glob("#thirdparty/libvpx/vp9/vp9_dx_iface.c")
    + Glob("#thirdparty/libvpx/vp9/vp9_iface_common.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_alloccommon.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_blockd.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_common_data.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_debugmodes.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_entropy.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_entropymode.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_entropymv.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_filter.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_frame_buffers.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_idct.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_loopfilter.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_mvref_common.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_pred_common.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_quant_common.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_reconinter.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_reconintra.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_rtcd.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_scale.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_scan.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_seg_common.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_thread_common.c")
    + Glob("#thirdparty/libvpx/vp9/common/vp9_tile_common.c")
    + Glob("#thirdparty/libvpx/vp9/decoder/vp9_decodeframe.c")
    + Glob("#thirdparty/libvpx/vp9/decoder/vp9_decodemv.c")
    + Glob("#thirdparty/libvpx/vp9/decoder/vp9_decoder.c")
    + Glob("#thirdparty/libvpx/vp9/decoder/vp9_detokenize.c")
    + Glob("#thirdparty/libvpx/vp9/decoder/vp9_dsubexp.c")
    + Glob("#thirdparty/libvpx/vp9/decoder/vp9_dthread.c")
    + Glob("#thirdparty/libvpx/vp9/decoder/vp9_job_queue.c")
    + Glob("#thirdparty/libvpx/vpx/src/vpx_codec.c")
    + Glob("#thirdparty/libvpx/vpx/src/vpx_decoder.c")
    + Glob("#thirdparty/libvpx/vpx/src/vpx_image.c")
    + Glob("#thirdparty/libvpx/vpx/src/vpx_psnr.c")
    + Glob("#thirdparty/libvpx/vpx_dsp/bitreader.c")
    + Glob("#thirdparty/libvpx/vpx_dsp/bitreader_buffer.c")
    + Glob("#thirdparty/libvpx/vpx_dsp/intrapred.c")
    + Glob("#thirdparty/libvpx/vpx_dsp/inv_txfm.c")
    + Glob("#thirdparty/libvpx/vpx_dsp/loopfilter.c")
    + Glob("#thirdparty/libvpx/vpx_dsp/prob.c")
    + Glob("#thirdparty/libvpx/vpx_dsp/vpx_convolve.c")
    + Glob("#thirdparty/libvpx/vpx_dsp/vpx_dsp_rtcd.c")
    + Glob("#thirdparty/libvpx/vpx_mem/vpx_mem.c")
    + Glob("#thirdparty/libvpx/vpx_scale/vpx_scale_rtcd.c")
    + Glob("#thirdparty/libvpx/vpx_scale/generic/yv12config.c")
    + Glob("#thirdparty/libvpx/vpx_scale/generic/yv12extend.c")
    + Glob("#thirdparty/libvpx/vpx_util/vpx_thread.c")
)
# if env["platform"] != "web":
#     sources += ["vp8/decoder/threading.c"]

# LIBOPUS: Remove /fp:strict flag
if env["CCFLAGS"]:
    if "/fp:strict" in env["CCFLAGS"]:
        env["CCFLAGS"].remove("/fp:strict")
        # Add /fp:precise flag
        env.Append(CCFLAGS=["/fp:precise"])

# LIBOPUS: Add instruction sets for architecture-specific optimizations (see https://github.com/godotengine/godot/blob/master/SConstruct#L798)
# These are added automatically by Godot, but since this is a GDExtension, we need to add them manually
if env["arch"] == "x86_64":
    if env.get("is_msvc", False):
        env.Append(CCFLAGS=["/d2archSSE42"])
    else:
        env.Append(CCFLAGS=["-msse4.2", "-mpopcnt"])
elif env["arch"] == "x86_32":
    if env.get("is_msvc", False):
        env.Append(CCFLAGS=["/arch:SSE2"])
    else:
        env.Append(CCFLAGS=["-msse2", "-mfpmath=sse", "-mstackrealign"])

# LIBOPUS: Add architecture-specific optimizations (see https://github.com/godotengine/godot-cpp/blob/master/tools/godotcpp.py#L182)
if env["platform"] == "windows" and env["arch"] in ["arm32", "arm64"]:
    # Skip optimizations on Windows ARM64, because `arm_neon.h` is not provided on setup-mingw (https://github.com/egor-tensin/setup-mingw)
    pass
elif env["arch"] == "x86_32":
    env.Append(CPPDEFINES=["OPUS_X32_OPT"])
    sources += [
        # "#thirdparty/libopus/celt/x86/celt_lpc_sse4_1.c", # SSE4.1 is disabled for x32
        # "#thirdparty/libopus/celt/x86/pitch_avx.c", # AVX2 is disabled for x32
        "#thirdparty/libopus/celt/x86/pitch_sse.c",
        "#thirdparty/libopus/celt/x86/pitch_sse2.c",
        # "#thirdparty/libopus/celt/x86/pitch_sse4_1.c", # SSE4.1 is disabled for x32
        "#thirdparty/libopus/celt/x86/vq_sse2.c",
        "#thirdparty/libopus/celt/x86/x86_celt_map.c",
        "#thirdparty/libopus/celt/x86/x86cpu.c",
        # "#thirdparty/libopus/silk/x86/NSQ_del_dec_avx2.c", # AVX2 is disabled for x32
        # "#thirdparty/libopus/silk/x86/NSQ_del_dec_sse4_1.c", # SSE4.1 is disabled for x32
        # "#thirdparty/libopus/silk/x86/NSQ_sse4_1.c", # SSE4.1 is disabled for x32
        # "#thirdparty/libopus/silk/x86/VAD_sse4_1.c", # SSE4.1 is disabled for x32
        # "#thirdparty/libopus/silk/x86/VQ_WMat_EC_sse4_1.c", # SSE4.1 is disabled for x32
        "#thirdparty/libopus/silk/x86/x86_silk_map.c",
        # "#thirdparty/libopus/silk/float/x86/inner_product_FLP_avx2.c", # AVX2 is disabled for x32
    ]
elif env["arch"] == "x86_64":
    env.Append(CPPDEFINES=["OPUS_X64_OPT"])
    sources += [
        "#thirdparty/libopus/celt/x86/celt_lpc_sse4_1.c",
        # "#thirdparty/libopus/celt/x86/pitch_avx.c", # AVX2 is disabled for x64
        "#thirdparty/libopus/celt/x86/pitch_sse.c",
        "#thirdparty/libopus/celt/x86/pitch_sse2.c",
        "#thirdparty/libopus/celt/x86/pitch_sse4_1.c",
        "#thirdparty/libopus/celt/x86/vq_sse2.c",
        "#thirdparty/libopus/celt/x86/x86_celt_map.c",
        "#thirdparty/libopus/celt/x86/x86cpu.c",
        # "#thirdparty/libopus/silk/x86/NSQ_del_dec_avx2.c", # AVX2 is disabled for x64
        "#thirdparty/libopus/silk/x86/NSQ_del_dec_sse4_1.c",
        "#thirdparty/libopus/silk/x86/NSQ_sse4_1.c",
        "#thirdparty/libopus/silk/x86/VAD_sse4_1.c",
        "#thirdparty/libopus/silk/x86/VQ_WMat_EC_sse4_1.c",
        "#thirdparty/libopus/silk/x86/x86_silk_map.c",
        # "#thirdparty/libopus/silk/float/x86/inner_product_FLP_avx2.c", # AVX2 is disabled for x64
    ]
elif env["arch"] == "arm32":
    env.Append(CPPDEFINES=["OPUS_ARM32_OPT"])
    # sources += []
elif env["arch"] == "arm64":
    env.Append(CPPDEFINES=["OPUS_ARM64_OPT"])
    sources += [
        "#thirdparty/libopus/celt/arm/arm_celt_map.c",
        "#thirdparty/libopus/celt/arm/armcpu.c",
        "#thirdparty/libopus/celt/arm/celt_neon_intr.c",
        "#thirdparty/libopus/celt/arm/pitch_neon_intr.c",
        "#thirdparty/libopus/silk/arm/arm_silk_map.c",
        "#thirdparty/libopus/silk/arm/biquad_alt_neon_intr.c",
        "#thirdparty/libopus/silk/arm/LPC_inv_pred_gain_neon_intr.c",
        "#thirdparty/libopus/silk/arm/NSQ_del_dec_neon_intr.c",
        "#thirdparty/libopus/silk/arm/NSQ_neon.c",
    ]

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("src/doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

copy = env.Install("{}/addons/WebmGdextension/bin/{}/".format(projectdir, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)
