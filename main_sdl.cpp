#include <string>
#include <utility>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

#include <stdio.h>
#include "Classes/framewrk/Mydefs.hpp"
#include "Classes/framewrk/Sprite.hpp"
#include "Classes/framewrk/audio.hpp"
#include "Classes/framewrk/Video.hpp"
#include "Classes/framewrk/movie.hpp"
#include "Classes/framewrk/Frm_int.hpp"
#include "Classes/moonchild/mc.hpp"
#include "Classes/moonchild/PREFS.HPP"
#include "Classes/moonchild/globals.hpp"

#include "mcsdl/mixer.hpp"

// stub Cmovie
Cmovie::Cmovie(void) : videoFilename(nullptr), videoReady(false) {}
Cmovie::Cmovie(Caudio *audio) {}
Cmovie::~Cmovie(void) {}
Smack *Cmovie::open(char *filename) { return {}; }
void Cmovie::close(Smack *smk) {}
void Cmovie::playtovideo(Smack *smk, Cvideo *video, Cblitbuf *hulpbuf, UINT16 zoomfactor) {}
UINT16 Cmovie::stillplaying(void) { return 0; }
void Cmovie::returnpal(BYTE *destpal) {}
void Cmovie::dump(FILE *fd) {}
// no need for private functions

// stub framewrk
void framework_util_SetMouse(int x, int y) {}
void framework_usefastfile(bool offon) {}
void framework_util_displayerror(char *errstring) {
  // stub
}
void __cdecl Err( LPSTR fmt, ... ) {
  // stub
}
HEARTBEAT_FN heartbeat;
bool gbGameLoop;
bool frmwrk_usefastfile;
char szAppName[1024];
char szCaption[1024];

// things used by video.cpp but not declared in frmwrk
unsigned short *SettingsPic;
unsigned short *ButPic[10];
unsigned short *SwitchPic[6];
unsigned short *SpeakerPic[2];
unsigned short *LoadingPic;
unsigned short *TempPic;

int frmwrk_CenterX;
int frmwrk_CenterY;
int g_CurDeltaX;
int g_CurDeltaY;
int g_MouseActualFlg;
int g_MouseFlg;
int g_MouseXCurrent;
int g_MouseYCurrent;
int g_MouseXDown;
int g_MouseYDown;
int g_SettingsFlg;

// iOS port functions not declared elsewhere
void ShowPicture(char *FileName) {}
// this static is safe because the game is not multithreaded
static std::string g_fullpath;
char *FullPath( char *a_File ) {
  const char* resource_path = SDL_GetEnvironmentVariable(SDL_GetEnvironment(), "MOONCHILD_RESOURCES");
  if (resource_path) {
    return nullptr;
  }
  std::string path = a_File;
  // idk weird hack to reuse FullPath in the audio mixer for now
  if (path.find("audio") != 0) {
    path = "moonchild/" + path;
  }
  g_fullpath = std::move(path);
  return (char*) g_fullpath.c_str();
}
char *FullWritablePath( char *a_File ) {
  return a_File;
}


// SDL specific stuff
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *screen_surface;
SDL_Texture *screen_texture;

// incorrect declarations in mc.hpp
extern HEARTBEAT_FN framework_InitGame(Cvideo *newvideo, Caudio *newaudio, Ctimer *newtimer, Cmovie *newmovie);
extern void framework_EventHandle(int event, int param);

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
  SDL_SetAppMetadata("Moon Child SDL", nullptr, "com.example.moonchildsdl");

  if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("Moon Child SDL", 640, 480, 0, &window, &renderer)) {
    return SDL_APP_FAILURE;
  }

  if (!SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX)) {
    return SDL_APP_FAILURE;
  }

  if (!SDL_SetRenderVSync(renderer, 1)) {
    return SDL_APP_FAILURE;
  }

  Cvideo *lvideo = new Cvideo();
  screen_surface = SDL_CreateSurface(640, 480, SDL_PIXELFORMAT_ARGB8888);
  if (!screen_surface) {
    return SDL_APP_FAILURE;
  }
  screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 640, 480);
  if (!screen_texture) {
    return SDL_APP_FAILURE;
  }

  SDL_Rect rect {};
  rect.w = 640;
  rect.h = 480;
  // Moon Child requires the screen buffer be at a fixed location in memory, so we have to use an intermediary surface.
  if (!SDL_LockSurface(screen_surface)) {
    return SDL_APP_FAILURE;
  }
  lvideo->on((unsigned char*) screen_surface->pixels, 640, 480, 256);
  SDL_UnlockSurface(screen_surface);

  Mixer_Init();

  heartbeat = framework_InitGame(lvideo, new Caudio(), new Ctimer(), new Cmovie());
  if (!heartbeat) {
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  if (!heartbeat) {
    return SDL_APP_SUCCESS;
  }
  SDL_Rect rect {};
  rect.w = 640;
  rect.h = 480;
  if (!SDL_LockSurface(screen_surface)) {
    return SDL_APP_FAILURE;
  }
  heartbeat = (HEARTBEAT_FN) (heartbeat)();
  if (!heartbeat) {
    return SDL_APP_SUCCESS;
  }
  SDL_UnlockSurface(screen_surface);

  // Copy screen surface (fixed location in memory) to texture surface (not fixed)
  SDL_Surface *txsfc;
  if (!SDL_LockTextureToSurface(screen_texture, &rect, &txsfc)) {
    return SDL_APP_FAILURE;
  }
  SDL_BlitSurface(screen_surface, &rect, txsfc, &rect);
  SDL_UnlockTexture(screen_texture);

  SDL_FRect src {};
  SDL_FRect dst {};
  src.w = 640;
  src.h = 480;
  dst.w = 640;
  dst.h = 480;
  SDL_RenderTexture(renderer, screen_texture, &src, &dst);

  SDL_FlushRenderer(renderer);
  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  switch (event->type) {
    case SDL_EVENT_QUIT: { return SDL_APP_SUCCESS; } break;
    case SDL_EVENT_KEY_DOWN: {
      switch (event->key.scancode) {
        case SDL_SCANCODE_UP: { framework_EventHandle(FW_KEYDOWN, prefs->upkey); } break;
        case SDL_SCANCODE_DOWN: { framework_EventHandle(FW_KEYDOWN, prefs->downkey); } break;
        case SDL_SCANCODE_LEFT: { framework_EventHandle(FW_KEYDOWN, prefs->leftkey); } break;
        case SDL_SCANCODE_RIGHT: { framework_EventHandle(FW_KEYDOWN, prefs->rightkey); } break;
        case SDL_SCANCODE_SPACE: { framework_EventHandle(FW_KEYDOWN, prefs->shootkey); } break;
        default: break;
      }
    }; break;
    case SDL_EVENT_KEY_UP: {
      switch (event->key.scancode) {
        case SDL_SCANCODE_UP: { framework_EventHandle(FW_KEYUP, prefs->upkey); } break;
        case SDL_SCANCODE_DOWN: { framework_EventHandle(FW_KEYUP, prefs->downkey); } break;
        case SDL_SCANCODE_LEFT: { framework_EventHandle(FW_KEYUP, prefs->leftkey); } break;
        case SDL_SCANCODE_RIGHT: { framework_EventHandle(FW_KEYUP, prefs->rightkey); } break;
        case SDL_SCANCODE_SPACE: { framework_EventHandle(FW_KEYUP, prefs->shootkey); } break;
        default: break;
      }
    }; break;
  }
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  framework_ExitGame();
  Mixer_Quit();

  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}
