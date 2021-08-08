# wadext

## Intro

A simple [WAD](https://en.wikipedia.org/wiki/Doom_WAD) extraction command line tool for [Doom engine (id Tech 1)](https://en.wikipedia.org/wiki/Doom_engine) mods.<br>

## Usage

`wadext [options] filename`

## Options

`-nogfxconvert` : Leave Doom format patches and flats in their original form, if not specified they will be converted to PNG.

`-nosndconvert` : Leave Doom format sounds in their original form, if not specified they will be converted to WAV.

`-heretic`, `-hexen`, `-strife`: Force usage of the named game's palette if the WAD does not contain one. Default is `Doom` palette

`-strip`: Remove node lumps from extracted maps.

`-tx`: Converts a set of `TEXTURE1/TEXTURE2/PNAMES` in the current directory to a textures.txt file.


## Compile instructions

git clone https://github.com/coelckers/wadext.git
cd wadext
cmake .
cmake --build .