# JkGfxMod

JkGfxMod makes Jedi Knight: Dark Forces II look better and run on new computers.

## Features

- Makes the game work on new computers
- Improves performance in certain scenarios
- Better quality of life:
    - HUD scaling
    - No mode switching between menus and gameplay
    - Hardware-accelerated windowed mode
- Better graphics:
    - 32-bit color
    - Gamma correction
    - Bloom
    - Ambient occlusion
    - Parallax mapping
- Better mods:
    - Support and tooling for advanced, high-quality materials

## Using JkGfxMod

**Note:** JkGfxMod is a standalone compatibility tool for Jedi Knight. JkGfxMod does not require any Windows compatibility settings, compatibility shims (installed by certain digital distributions), administrative privileges, or DirectDraw wrappers (e.g. dgVoodoo). We recommend disabling these compatibility fixes before using JkGfxMod.

Please follow the [installation guide](doc/install.md) for your version of the game.

## Installing material packs (optional)

Copy the material pack directory to the `jkgm/materials` directory in your Jedi Knight directory.

## Creating material packs (optional)

Material packs must be compiled using the `compile` utility.

- Create a source directory and script file
    - See `assets/sample_matpack` for a working example
- Run the compile utility
    - Example: `compile --script script.json --output outdir --resource path_to_jk/resource`

After the `compile` command completes, the finished material pack will be inside the output directory.

For more information, please see the [material compiler reference](doc/compile.md).

## Common problems

### Materials don't show up in-game

If you properly install an optional material pack and the textures are not showing up, try setting the brightness slider all the way to min.  The brightness slider can be found in Setup → Display → Brightness.

### Textures look scrambled after loading a save

Start a new game. Save files in Jedi Knight: Dark Forces II cannot be reused after installing or removing most mods.

### Stuttering during gameplay

JkGfxMod streams materials from storage as they are needed. This may cause some stuttering, especially if JkGfxMod materials are installed on a slow hard disk. This can be corrected by adding a `vram_texture_preload_size` entry to the configuration file. Please see the [configuration guide](doc/config.md) for more information.

## Issues, questions, and feedback

Please follow these steps to report issues:

- Reproduce the issue with logging enabled
    - Set the `JKGM_LOG_FILE=jkgm.log` environment variable, OR
    - Set the `log_path` configuration option
- Report the issue on [GitHub](https://github.com/jdmclark/jkgfxmod/issues)
    - Include a brief explanation of what you were doing when the problem happened
    - Attach the log file to the issue
