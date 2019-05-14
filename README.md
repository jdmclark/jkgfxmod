# JkGfxMod

JkGfxMod is a graphics enhancement and compatibility tool for Jedi Knight: Dark Forces II.

## Features

- Improves compatibility with modern versions of Windows
- Improves game performance in certain scenarios
- Renders the game in full 32-bit color
- Adds screen-space ambient occlusion
- Adds support for materials
    - High-resolution/high-color textures
    - Emissive textures
    - Parallax occlusion mapping
    - Requires a material pack (not included)
- Corrects several graphics issues in the base game:
    - Transparent surfaces are now ordered correctly
    - Fixes dark fringes around color keyed surfaces
    - Adds gamma correct rendering

## Limitations

This is an alpha release. It is very likely that you will encounter issues while using this tool.

JkGfxMod has only been tested with the CD-ROM version of Jedi Knight: Dark Forces II using Windows 10. JkGfxMod may not function correctly on other operating systems, or with other versions of the game. Please report any version or platform-specific incompatibilities.

## Using JkGfxMod

**Note:** JkGfxMod is a standalone compatibility tool for Jedi Knight. JkGfxMod enables hardware-accelerated graphics in Jedi Knight without Windows compatibility settings, shims (installed by certain digital distributions), administrative privileges, or other DirectDraw wrappers (e.g. dgVoodoo). We recommend disabling these compatibility fixes before using JkGfxMod.

- Extract the distribution package into your Jedi Knight directory
- Edit the config file: `jkgm.json`
    - You must specify your desired resolution in this file.
    - Your desktop color depth is used automatically and does not need to be configured.
- Run `inject.exe`
- Enable hardware acceleration in the display options
    - The Jedi Knight display options screen is hard-coded to say 16-bit color. Ignore this. JkGfxMod will always use your desktop color depth.

## Editing configuration file (`jkgm.json`)

**resolution**: What resolution to output at. Syntax: `[ x, y ]`. Default: `[ 1024, 768 ]`. 
**internal_resolution**: What resolution to render at. Syntax: `{unknown}`, or `null` to use the same resolution as output. Default: `null`.
**fullscreen**: Select whether to use fullscreen or Windowed mode. Syntax: boolean (`true` for fullscreen, or `false` for windowed).
**hud_scale**: Upscale the HUD to avoid tiny HUD at high resolutions. Use whole integers for best results. Syntax: decimal. Recommend values (based on 360p HUD size and Jedi Outcast/Academy HUD sizes): `2.0` for 720p, `3.0` for 1080p, `4.0` for 1440p, `6.0` for 2160p. Default: `1.0`.
**max_anisotropy**: Level of anisotropic filtering to use. Can be used even when `enable_texture_filtering` is set to false. Syntax: `1.0` to disable, `2.0` for 2x, `4.0` for 4x, `8.0` for 8x, `16.0` for 16x. Default: `2.0`.
**enable_bloom**: Use bloom effect for brighter lights. Requires compatible texture pack. Syntax: boolean. Default: `true`. 
**enable_ssao**: Use screen-space ambient occlusion to subtly increase perceived depth in the image. Default: `true`. 
**enable_parallax**: Use parallax occlusion mapping to add depth to individual textures. Requires compatible texture pack. Syntax: boolean. Default: `true`.
**enable_texture_filtering**: Use bilinear filtering on textures, reducing texture aliasing whilst also making closeup textures blurry. Does not affect trilinear filtering (mipmapping) or anisotropic filtering. Syntax: boolean. Default: `true`.
**enable_posterized_lighting**: Posterize the lighting, introducing noticeable banding that simulates the 8-bit lighting of the RenderDroid software renderer. Syntax: boolean. Default: `false`.
**command**: Name of Jedi Knight or Mysteries of the Sith executable to use. Syntax: `"<filename>"`. Default: `"jk.exe"`.

## Installing material packs

Copy the material pack directory to the `jkgm/materials` directory in your Jedi Knight directory.

## Creating material packs

Material packs must be compiled using the `compile` utility.

- Create a source directory and script file
    - Not currently documented
    - See `assets/sample_matpack` for a working example
- Run the compile utility
    - Example: `compile --script script.json --output outdir --resource path_to_jk/resource`

After the `compile` command completes, the finished material pack will be inside the output directory.

For more information, please see the [material compiler reference](doc/compile.md).

## Issues, questions, and feedback

Please follow these steps to report issues:

- Reproduce the issue with logging enabled
    - Rerun `inject.exe` with the `JKGM_LOG_FILE=jkgm.log` environment variable set
- Report the issue on [GitHub](https://github.com/jdmclark/jkgfxmod/issues)
    - Include a brief explanation of what you were doing when the problem happened
    - Attach the log file to the issue
