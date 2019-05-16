# Installing JkGfxMod

Installation instructions vary depending on the game version. Please follow the instructions in the correct section, below.

## Table of Contents

* [Jedi Knight: Dark Forces II](#jedi-knight-dark-forces-ii)
    * [Original CD-ROM](#jedi-knight-cd-rom)
    * [Steam](#jedi-knight-steam)
    * [GOG Galaxy](#jedi-knight-gog-galaxy)
* [Jedi Knight: Mysteries of the Sith](#jedi-knight-mysteries-of-the-sith)
    * [Original CD-ROM](#mysteries-of-the-sith-cd-rom)
    * [Steam](#mysteries-of-the-sith-steam)
    * [GOG Galaxy](#mysteries-of-the-sith-gog-galaxy)
    
## Jedi Knight: Dark Forces II

### Jedi Knight (CD-ROM)

* Extract the distribution package into your Jedi Knight directory
* Edit the config file: `jkgm.json`
    * You must specify your desired resolution in this file
    * Your desktop color depth is used automatically and does not need to be configured
    * See the [configuration guide](config.md) for more information
* Run `inject.exe` from your Jedi Knight directory
* Enable hardware acceleration in the display options
    * The Jedi Knight display options screen is hard-coded to say 16-bit color. Ignore this. JkGfxMod will always use your desktop color depth.

Installation is now complete. You can run Jedi Knight with JkGfxMod by running `inject.exe` from your Jedi Knight directory.

### Jedi Knight (Steam)

This installation method will allow you to use JkGfxMod with the Steam overlay and soundtrack fix.

* Obtain the original CD-ROM version 1.0 `jk.exe`
    * This is required; the Steam executable does not work with JkGfxMod
* Rename the above `jk.exe` file `jkoriginal.exe`
* Put `jkoriginal.exe` in your Steam `Star Wars Jedi Knight` app directory
* Extract the distribution package into your Steam `Star Wars Jedi Knight` app directory
* Rename `inject.exe` to `JediKnight.exe`
* Edit the config file: `jkgm.json`
    * Change the `command` configuration option to `jkoriginal.exe`
    * You must also specify your desired resolution in this file
    * Your desktop color depth is used automatically and does not need to be configured
    * See the [configuration guide](config.md) for more information
* Run Jedi Knight from Steam
* Enable hardware acceleration in the display options
    * The Jedi Knight display options screen is hard-coded to say 16-bit color. Ignore this. JkGfxMod will always use your desktop color depth.

Installation is now complete. You can run Jedi Knight with JkGfxMod by running Jedi Knight from Steam.

### Jedi Knight (GOG Galaxy)

This installation method will allow you to use JkGfxMod from GOG Galaxy.

* Obtain the original CD-ROM version 1.0 `jk.exe`
    * This is required; the GoG Galaxy executable (1.0.1) does not work with JkGfxMod
* Rename the above `jk.exe` file `jkoriginal.exe`
* Put `jkoriginal.exe` in your `Star Wars Jedi Knight - Dark Forces 2` app directory
* Extract the distribution package into your `Star Wars Jedi Knight - Dark Forces 2` app directory
* Rename `JKStart.exe` to `JKStartOld.exe`
* Rename `inject.exe` to `JKStart.exe`
* Edit the config file: `jkgm.json`
    * Change the `command` configuration option to `jkoriginal.exe`
    * You must also specify your desired resolution in this file
    * Your desktop color depth is used automatically and does not need to be configured
    * See the [configuration guide](config.md) for more information
* Run Jedi Knight from GOG Galaxy
* Enable hardware acceleration in the display options
    * The Jedi Knight display options screen is hard-coded to say 16-bit color. Ignore this. JkGfxMod will always use your desktop color depth.

Installation is now complete. You can run Jedi Knight with JkGfxMod by running Jedi Knight from GOG Galaxy.

## Jedi Knight: Mysteries of the Sith

### Mysteries of the Sith (CD-ROM)

* Extract the distribution package into your Mysteries of the Sith directory
* Edit the config file: `jkgm.json`
    * Change the `command` configuration option to `jkm.exe`
    * You must also specify your desired resolution in this file
    * Your desktop color depth is used automatically and does not need to be configured
    * See the [configuration guide](config.md) for more information
* Run `inject.exe` from your Mysteries of the Sith directory
* Enable hardware acceleration in the display options
    * The Mysteries of the Sith display options screen is hard-coded to say 16-bit color. Ignore this. JkGfxMod will always use your desktop color depth.

Installation is now complete. You can run Mysteries of the Sith with JkGfxMod by running `inject.exe` from your Mysteries of the Sith directory.

### Mysteries of the Sith (Steam)

This installation method will allow you to use JkGfxMod with the Steam overlay and soundtrack fix.

* Obtain the original CD-ROM version 1.0 `jkm.exe`
    * This is required; the Steam executable does not work with JkGfxMod
* Apply a no-CD patch to `jkm.exe`
    * This is required unless you have a CD-ROM drive and the original games
* Rename the above `jkm.exe` file `jkmoriginal.exe`
* Put `jkmoriginal.exe` in your Steam `Jedi Knight Mysteries of the Sith` app directory
* Extract the distribution package into your Steam `Jedi Knight Mysteries of the Sith` app directory
* Rename `inject.exe` to `JediKnightM.exe`
* Edit the config file: `jkgm.json`
    * Change the `command` configuration option to `jkmoriginal.exe`
    * You must also specify your desired resolution in this file
    * Your desktop color depth is used automatically and does not need to be configured
    * See the [configuration guide](config.md) for more information
* Run Mysteries of the Sith from Steam
* Enable hardware acceleration in the display options
    * The Mysteries of the Sith display options screen is hard-coded to say 16-bit color. Ignore this. JkGfxMod will always use your desktop color depth.

Installation is now complete. You can run Mysteries of the Sith with JkGfxMod by running Mysteries of the Sith from Steam.

### Mysteries of the Sith (GOG Galaxy)

This installation method will allow you to use JkGfxMod from GOG Galaxy.

* Obtain the original CD-ROM version 1.0 `jkm.exe`
    * This is required; the GoG Galaxy executable (1.0) does not work with JkGfxMod
* Apply a no-CD patch to `jkm.exe`
    * This is required unless you have a CD-ROM drive and the original games
* Rename the above `jkm.exe` file `jkmoriginal.exe`
* Put `jkmoriginal.exe` in your `Star Wars Jedi Knight - Mysteries of the Sith` app directory
* Extract the distribution package into your `Star Wars Jedi Knight - Mysteries of the Sith` app directory
* Rename `JKMStart.exe` to `JKMStartOld.exe`
* Rename `inject.exe` to `JKMStart.exe`
* Edit the config file: `jkgm.json`
    * Change the `command` configuration option to `jkmoriginal.exe`
    * You must also specify your desired resolution in this file
    * Your desktop color depth is used automatically and does not need to be configured
    * See the [configuration guide](config.md) for more information
* Run Mysteries of the Sith from GOG Galaxy
* Enable hardware acceleration in the display options
    * The Mysteries of the Sith display options screen is hard-coded to say 16-bit color. Ignore this. JkGfxMod will always use your desktop color depth.

Installation is now complete. You can run Mysteries of the Sith with JkGfxMod by running Mysteries of the Sith from GOG Galaxy.
