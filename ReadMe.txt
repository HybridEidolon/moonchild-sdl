  Moon Child
iPhone version

Original version (1997):
PC - Windows 95 - DirectX 5


I could have released the original code but that is a lot less portable than this version.
The original version uses DirectX for its graphics. I removed all of it and fixed it to use pixelbuffers everywhere.
So each tile/sprite is a W*H*RGBA buffer. The game itself is 640*480*RGBA. So where GPU was used, I now just copy pixels over.
That leaves us with a 640*480*4 pixelbuffer that needs to be drawn on the screen.
In the iOS version I do this by creating a texture from the pixelbuffer. That texture I center on the screen. This opens the door for potential shader effects.

Moon Child consists of 2 parts:
  - The framework
  - The actual game

The framework was the place where it had the code to do video, sprites, blitting, music, soundeffects, etc
The game was just the game logic.

So it's mostly the framework you need to alter if you wish to port this to any other platform. 

The game itself need to be called 60 times a second. It does this via a method called 'heartbeat'. This is a function pointer that after the 60hz tick needs to point to the next thing it needs to call 60 times a second. 
So the game has an entrypoint for the actual game, for loading a level, for the hiscore, for the level select, for the gameover, etc.  The 'contract' is that each of these 'heartbeat' functions need to return the next heartbeat to run in the loop.

The mac port has some 'Objective C++' code. These are the .mm files.  Objective C++ is a very cool thing where you can mnix Objective C code with C++ code. That is superpowerful as you can then do apple specific stuff (like their view logic) and calling the game (which is all C++).

Anyhow, most of the Apple glue/ugliness is mostly contained in the view and viewcontroller. (Less so in the framework code...  the framework code, is just doing everything in pixelbuffers).

So how does the UI / shell interact with the game:
key events kan be send to the game like this:  
framework_EventHandle(FW_KEYDOWN,prefs->upkey);    (tells the game the up key is pressed)
framework_EventHandle(FW_KEYUP,prefs->upkey);    (tells the game the up key is let go off)

framework_EventHandle(FW_KEYDOWN,(int) prefs->shootkey).    (shootkey is the keycode for the 'action button'. Which is shooting in world 3, and toggling switches in world 1)

prefs is an exported struct by the game that holds the current settings... among which are the key codes. So if the user changes these, make sure to set them.

For cheat, you can alter this exported variable from the game:  maxlevel. If you set it to 0, then the furthest Moon Child has achieved is level 0. If you se t it to 12, All levels will be unlocked right away.  

To quit from a level you need to send the 'Q' key:
#define VK_ESCAPE 'Q'
framework_EventHandle(FW_KEYDOWN,(int) VK_ESCAPE);

----

The music historically was streamed from CD. On the mac is just plays an MP3 instead.

----

All assets were combined in 1 big file 'called fastfile'. This dramaticaly speeded up loading assets.

All graphics are in PCX format. Ancient fileformat which is very easy to load. 

-----
The iphone doesn't support the movies. Yes MC also has movies. But they were in an ancient proprietary format called 'smacker'. Anyhow, the code is still ion the game. It's one of the heartbeats. 

----

Also fun, the game has a built in level editor. So that might be fun to open up. you can enable it by uncommenting this line in. mc.cpp: //#define EDITOR
It historically used the mouse to scroll through the level. where you exit the editor it places mc so you can immediatly test. Left mouse buttons places tiles, and right mouse button goes to the tile selection screen.     judging by the code you probably need to feed: INT32 mousex = 10;  and INT32 mousey = 10; with the correct mouse coordinates.



Enjoy!

Reinier van Vliet
www.proofofconcept.nl
