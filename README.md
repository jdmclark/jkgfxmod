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
