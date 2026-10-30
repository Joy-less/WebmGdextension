# WebM GDExtension

[![Godot Asset Store](https://img.shields.io/github/v/release/Joy-less/WebmGdextension.svg?label=Godot%20Asset%20Store&logo=godotengine)](https://store.godotengine.org/asset/joyless/webm-gdextension)

This addon lets you import WebM, a modern video format with excellent compression, in Godot 4.

WebM GDExtension adds `VideoStreamWebm`, which is almost identical in features to the built-in `VideoStreamTheora`.

The supported video codec is VP9. The VP8 video codec is disabled, but it can be re-enabled easily if desired.
The supported audio codecs are Opus and Vorbis.

> [!WARNING]
> This addon decodes WebM files on the CPU without architecture-specific optimizations or hardware acceleration,
> making it very slow. This addon mostly exists as an initial way to get WebM working in Godot before the
> [WebM pull request](https://github.com/godotengine/godot/pull/119299) is merged.

## Features

- Import `.webm` files in the editor.
- Play the `.webm` files in the editor inspector.
- Load `.webm` files at runtime.
- Play `.webm` files at runtime.

## How to Use

To convert an MP4 file to WebM with VP9 at constant quality 30 and Opus at 128 kbits/s, install [FFmpeg](https://ffmpeg.org) and run:
```
ffmpeg -i input.mp4 -c:v libvpx-vp9 -crf 30 -b:v 0 -row-mt 1 -c:a libopus -b:a 128k output.webm
```

To also downscale to 720p using Lanczos:
```
ffmpeg -i input.mp4 -vf "scale=1280x720:flags=lanczos" -c:v libvpx-vp9 -crf 30 -b:v 0 -row-mt 1 -c:a libopus -b:a 128k output.webm
```

## Recommended Quality

See [Google's Recommended Settings](https://developers.google.com/media/vp9/settings/vod?utm_source=chatgpt.com#quality) for the recommended VP9 quality level.

See [Opus GDExtension](https://github.com/Joy-less/OpusGdextension) for the recommended Opus bitrate.

## Supported Platforms

WebM GDExtension has a baseline of Godot 4.5 and aims to support every platform supported by Godot 4.5.

Note: Every supported platform has binaries for single (but not double) precision and debug/release mode.

| Platform | Supported | SIMD Optimizations | Hardware Acceleration |
| --- | --- | --- | --- |
| Windows x64 | ✅ | ❌ (1) | ❌ (2) |
| Windows x32 | ✅ | ❌ (1) | ❌ (2) |
| Windows ARM64 | ✅ | ❌ (1) | ❌ (2) |
| Linux x64 | ✅ | ❌ (1) | ❌ (2) |
| Linux x32 | ✅ | ❌ (1) | ❌ (2) |
| Linux ARM64 | ✅ | ❌ (1) | ❌ (2) |
| Linux ARM32 | ✅ | ❌ (1) | ❌ (2) |
| macOS Universal | ✅ | ❌ (1) | ❌ (2) |
| Android x64 | ✅ | ❌ (1) | ❌ (2) |
| Android x32 | ✅ | ❌ (1) | ❌ (2) |
| Android ARM64 | ✅ | ❌ (1) | ❌ (2) |
| Android ARM32 | ✅ | ❌ (1) | ❌ (2) |
| iOS ARM64 | ✅ | ❌ (1) | ❌ (2) |
| Web WASM32 | ✅ | ❌ (1) | ❌ (2) |

*(1) WebM GDExtension does not use the available SIMD optimizations for decoding video, because the aim is to get a pure-C implementation working in a bug-free state before selectively re-adding the architecture-specific optimizations in the future.*

*(2) WebM GDExtension does not use hardware acceleration for decoding video, instead relying only on the CPU. The aim is for a contributor to add hardware acceleration in the future after WebM is merged into Godot.*

See [Opus GDExtension](https://github.com/Joy-less/OpusGdextension) for the supported platforms for Opus SIMD optimizations.

## Proposal

There is a [proposal](https://github.com/godotengine/godot-proposals/issues/15617) and [pull request](https://github.com/godotengine/godot/pull/119299) to add WebM into Godot core.

Please give them a thumbs up (👍) so it's more likely to be merged!

## Credits

To use this library, you need to follow the license of the following repositories:
- [WebM GDExtension by Joyless (MIT license)](https://github.com/Joy-less/WebmGdextension)
- [Ogg by Xiph (BSD-3-Clause license)](https://github.com/xiph/ogg)
- [Opus by Xiph (BSD-3-Clause license)](https://github.com/xiph/opus)
- [Vorbis by Xiph (BSD-3-Clause license)](https://www.xiph.org/vorbis)
- [libvpx by The WebM Project authors (BSD-3-Clause license)](https://github.com/webmproject/libvpx)
- [libsimplewebm by Błażej Szczygieł and DaveTheEggman (MIT license)](https://github.com/The-Simplicity-Group/libsimplewebm)
- [YUV2RGB by Robin Watts (BSD license)](http://wss.co.uk/pinknoise/yuv2rgb)
- [Godot by Godot Engine contributors (MIT license)](https://github.com/godotengine/godot)
