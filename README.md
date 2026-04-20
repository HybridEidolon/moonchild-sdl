# Moon Child SR

This is a version of the 1997 Windows 95 game Moon Child adjusted and modernized, itself based on the original team's SDL2 source release on April 19, 2026. It specifically focuses on portability and features supporting speedrunners.

## New features over original release

- Gamepad integration via SDL
- Fullscreen toggle (alt+enter)
- 22kHz sound effects from 1997 Windows version (ripped by @cookedmonty.bsky.social from the EXE)
- Full support for XDG paths and env overrides:
  - MOONCHILD_ASSETS_PATH, XDG_DATA_HOME, XDG_DATA_DIRS for assets. Falls back to SDL base path and working directory.
  - MOONCHILD_SAVE_PATH, XDG_CONFIG_HOME for options and high scores
  - Note, this means opts/high dats are saved to the user's home directory in `.config/moonchild` by default.

### Planned features

(bit of a stretch to say "planned", only for as much as this [strange mood][sm] lasts)

- Restored video playback
- Precise game timers for IL and total time
- [LiveSplit Server][lss] integration
- [Archipelago](https://archipelago.gg/) mode
- 50Hz/60Hz toggle; the game likely was supposed to run at 50Hz, not 60
- Remove SDL_mixer and use a small inline mixer w/ minimp3 instead

[sm]: https://dwarffortresswiki.org/index.php/Strange_mood
[lss]: https://github.com/LiveSplit/LiveSplit/blob/master/README.md#the-livesplit-server

## Building

As of this writing Release builds have misoptimizations with clang that prevent them from being used. Debug builds are fine as-is.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

---

Original README.md below.

#  Moon Child
SDL portable version

Original version (1997):
PC - Windows 95 - DirectX 5



### Running

# Compile on MAC
From the terminal:
brew install cmake sdl2 sdl2_mixer
cmake -S . -B build && cmake --build build

# Compile on PC   (Couln't test...but should be something like)

- install SDL2 + SDL2_mixer + zlib + a C/C++ toolchain + CMake,
- cmake -S . -B build && cmake --build build

# Compile on Linux  (Couln't test...but should be something like)
- (Debian Ubuntu): sudo apt install cmake build-essential libsdl2-dev libsdl2-mixer-dev zlib1g-dev
- (Fedora): sudo dnf install cmake gcc-c++ SDL2-devel SDL2_mixer-devel zlib-devel
- (Arch): sudo pacman -S cmake base-devel sdl2 sdl2_mixer zlib


# To run:
./build/moonchild_shell 


### Porting tips & tricks

I could have released the original code but that is a lot less portable than this version.
The original version uses DirectX for its graphics. I removed all of it and fixed it to use pixelbuffers everywhere.
So each tile/sprite is a W * H * RGBA buffer. The game itself is 640 * 480 * RGBA. So where GPU was used, I now just copy pixels over.
That leaves us with a 640 * 480 * 4 pixelbuffer that needs to be drawn on the screen.
In the SDL version I do this by creating a texture from the pixelbuffer. That texture I center on the screen. This opens the door for potential shader effects.

Moon Child consists of 2 parts:
  - The framework
  - The actual game

The framework was the place where it had the original code to do video, sprites, blitting, music, soundeffects, etc
The game was just the game logic.

So it's mostly the framework you need to alter if you wish to port this to any other platform. Or use the SDL as a starting point. This is why I ported it to SDL, to make it easier to port.

The game itself needs to be called 60 times a second. It does this via a method called 'heartbeat'. This is a function pointer that after the 60hz tick needs to point to the next thing it needs to call 60 times a second. 
So the game has an entrypoint for the actual game, for loading a level, for the hiscore, for the level select, for the gameover, etc.  The 'contract' is that each of these 'heartbeat' functions need to return the next heartbeat to run in the loop.

So how does the UI / shell interact with the game:
key events kan be send to the game like this:  
framework_EventHandle(FW_KEYDOWN,prefs->upkey);    (tells the game the up key is pressed)
framework_EventHandle(FW_KEYUP,prefs->upkey);    (tells the game the up key is let go off)

framework_EventHandle(FW_KEYDOWN,(int) prefs->shootkey).    (shootkey is the keycode for the 'action button'. Which is shooting in world 3, and toggling switches in world 1)

prefs is an exported struct by the game that holds the current settings... among which are the key codes. So if the user changes these, make sure to set them.

For cheat, in the SDL wrapper I have added a method to enable or disable cheat.   For now it is disabled by default.

To quit from a level you need to send the 'Q' key:
#define VK_ESCAPE 'Q'
framework_EventHandle(FW_KEYDOWN,(int) VK_ESCAPE);

### Music/sound

The music historically was streamed from CD. In this version it just plays an MP3 instead.
The sounds are wav's

In DirectX each soundobject could be assigned a volume and a pan (stereo position). However in SDL it works different. Here when you play a sound it assigns a channel, and then later you need to pan and volume that channel. I' quickly hacked a soundobject->channel lookuptable. Very ugly, but at least I didn't need to rewrite the game.,

### Graphics

All assets were combined in 1 big file 'called fastfile'. This dramaticaly speeded up loading assets.

All graphics are in PCX format. Ancient fileformat which is very easy to load. 

Bigger graphics are in TGA. Why?  Don't know anymore.

### Movies

Yes MC also has movies. But they were in an ancient proprietary format called 'smacker'. Anyhow, the code is still in the game. It's one of the heartbeats. However since I can't play the movies it skips it now.    I can see if in a later stage I can replace it for an mp4 version somehow.

### Level editor

Also fun, the game has a built in level editor. So that might be fun to open up. you can enable it by uncommenting this line in. mc.cpp: //#define EDITOR
then if you press the 'e' key it starts the editor. And press the 'p' key it shows a pattern/tile page. Select a tile or block and you can edit the level.
The tiny digits on the patterns are thew tile properties. So it determines if you fall through it, can jump through it from the bottom, or slow down, etc etc

The mouse moves the cursor. If you stop the editor, the place where the cursor was is the place where moon child will reappear.

### Credits
Enjoy!

Original game code (and porting code)
Reinier van Vliet
<https://www.proofofconcept.nl>

Graphics
Metin Seven
<https://www.metinseven.nl>

Music & Soundfx
Ramon Braumuller
<https://open.spotify.com/artist/6ljLO5A329ym1FARh4xAz4?si=I2-mmFi4Qq-CLNZvoku7Pw>

