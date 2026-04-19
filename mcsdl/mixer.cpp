#include "mixer.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
# include <Windows.h>
#else
# ifndef _LARGEFILE64_SOURCE
#  define _LARGEFILE64_SOURCE
# endif
# include <sys/mman.h>
# include <sys/types.h>
# include <fcntl.h>
# include <unistd.h>
#endif

#include <SDL3/SDL.h>

#include "minimp3.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#include <immintrin.h>
#define NEED_INTEL_DENORMAL_BIT 1
#endif

// forward decs for functions not declared in a header
char *FullPath( char *a_File );

static std::vector<const char*> soundFilenames =
{
    "", //0
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",  //15
    "mcskid.wav",  //16
    "spring.wav", //17
    "mcwall.wav", //18
    "mcdrain.wav", //19
    "mcfall.wav", //20
    "blub.wav", //21
    "door.wav",  //22
    "switch.wav", //23
    "bonus.wav", //24
    "warp.wav", //25
    "shoot.wav",  //26
    "rumble.wav", //27
    "waterval.wav", //28
    "mcloop1.wav", //29
    "mcloop2.wav", //30
    "sapstap1.wav", //31
    "sapstap2.wav", //32
    "helmlp1.wav", //33
    "helmlp2.wav", //34
    "houtpunt.wav", //35
    "ball.wav", //36
    "segmshot.wav", //37
    "segmexpl.wav", //38
    "segmhit.wav", //39
    "vlamwerp.wav",  //40
    "tanden.wav", //41
    "tandenm.wav", //42
    "camstart.wav",  //43
    "camstop.wav",  //44
    "cammove.wav",  //45
    "madeit.wav", //46
    "heks.wav", //47
    "graspod.wav", //48
    "bat.wav", //49
    "vogel.wav", //50
    "vuur.wav", //51
    "drilboor.wav", //52
    "loopband.wav", //53
    "ventltor.wav", //54
    "bee.wav", //55
    "bee2.wav", //56
    "ptoei.wav", //57
    "schuif.wav", //58
    "smexp.wav", //59
    "backpak.wav", //60
    "restart.wav", //61
    "bigexp.wav", //62
    "stroom.wav", //63
    "cannon.wav", //64
    "gewicht.wav", //65
    "wheel.wav", //66
    "appel.wav", //67
    "mcdood.wav", //68
    "mcfart.wav", //69
    "chemo.wav", //70
    "tik.wav", //71
    "tak.wav",//72
    "raket.wav", //73
    "chemo2.wav", //74
    "helmdood.wav", //75
    "ketting.wav", //76
    "dimndsht.wav", //77
    "glasblok.wav", //78
    "hyprlift.wav", //79
    "lightwav.wav", //80
    "morphsht.wav", //81
    "mushup.wav", //82
    "mushdwn.wav", //83
    "plntlft.wav", //84
    "pltfdwn.wav", //85
    "pltfup.wav", //86
    "qbert1.wav", //87
    "qbert2.wav", //88
    "roltnlp.wav", //89
    "slowlift.wav", //90
    "tangjmp.wav", //91
    "tangclos.wav", //92
    "woeiwoei.wav", //93
    "heatskr.wav", //94
    "demo.wav", //95
    "explo.wav",
    "mcdrainold.wav"
};

namespace {
struct SoundChunk {
  std::vector<float> data;
  INT32 length = 0;
  INT32 active = 0;
  INT32 poly = 1;
};
struct SoundChannel {
  bool active = false;
  bool looping = false;
  HSNDOBJ id;
  INT32 pos;
  INT32 vol;
  INT32 pan;
};
};

// memory mapped IO, for easy music reading
namespace {
struct memmapped {
  void* data;
  unsigned long long size;
  bool valid;
#ifdef _WIN32
  HANDLE hfile;
  HANDLE hmap;
#else
  int fd;
#endif
};
}

static memmapped map_file(const char* path) {
  memmapped m {};

#ifdef _WIN32
  m.hfile = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if (m.hfile == INVALID_HANDLE_VALUE) {
    return m;
  }
  LARGE_INTEGER filesize;
  GetFileSizeEx(m.hfile, &filesize);
  m.size = filesize.QuadPart;
  m.hmap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
  if (m.hmap == INVALID_HANDLE_VALUE) {
    CloseHandle(h.hfile);
    h.hfile = INVALID_HANDLE_VALUE;
    return m;
  }
  m.data = MapViewOfFile((*mmapped)->hMap, FILE_MAP_READ, 0, 0, 0);
  m.valid = true;

#else
  int fd;
  if ((fd = open(path, O_RDONLY)) == -1) {
    return m;
  }
  off_t filesize = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);

  m.data = mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
  m.fd = fd;
  m.size = filesize;
  m.valid = true;

#endif

  return m;
}

static void close_mapped_file(memmapped& m) {
#ifdef _WIN32
  UnmapViewOfFile(m.data);
  CloseHandle(m.hmap);
  CloseHandle(m.hfile);
  m = {};
#else
  munmap(m.data, m.size);
  close(m.fd);
  m = {};
#endif
}

static SDL_AudioStream* output_stream;

static HSNDOBJ chunk_counter;
static std::unordered_map<HSNDOBJ, SoundChunk> chunk_map;
static std::array<SoundChannel, 64> sound_channels;
static bool music_active;
static mp3dec_t music_decoder;
static memmapped music_file;
static unsigned long long music_pos;
static SDL_AudioStream *music_stream;
static float music_gain;

static std::array<float, 8192> mixbuffer;

static float calcVolume(INT32 volume)
{
    if(volume < -4000)volume = -4000;
    if(volume > 0) volume = 0;
    float gain = (volume+4000) / 4000.0f;
    return gain;
}

static float calcPan(INT32 pan)
{
    if(pan < -1000)pan = -1000;
    if(pan >= 1000) pan = 1000;
    float panf = pan / 1000.0f;
    return panf;
}

static void audio_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
  int add_samples = additional_amount / (sizeof(float) * 2); // stereo f32
#ifdef NEED_INTEL_DENORMAL_BIT
  INT32 fpustate = 0;
  fpustate |= _MM_GET_FLUSH_ZERO_MODE() | _MM_FLUSH_ZERO_ON ? 1 : 0;
  fpustate |= _MM_GET_DENORMALS_ZERO_MODE() == _MM_DENORMALS_ZERO_ON ? 2 : 0;

  if ((fpustate & 1) == 0) {
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  }
  if ((fpustate & 2) == 0) {
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
  }
#endif

  mixbuffer = {};

  // decode music stream
  if (music_active) {
    SDL_GetAudioStreamData(music_stream, mixbuffer.data(), add_samples * sizeof(float) * 2);
  }

  // mix sounds
  for (auto& channel : sound_channels) {
    if (!channel.active) {
      continue;
    }
    SoundChunk& chunk = chunk_map[channel.id];
    if (chunk.length == 0) {
      continue;
    }
    int to_mix;
    if (channel.looping) {
      to_mix = add_samples;
    } else {
      to_mix = std::min<int>(add_samples, std::max<int>(chunk.length - channel.pos, 0));
    }
    int mix_pos = 0;
    while (channel.active && channel.pos < chunk.length && to_mix > 0 && mix_pos < mixbuffer.size()) {
      // TODO panning, volume
      // mixbuffer is interleaved stereo
      float gain = calcVolume(channel.vol);
      float pan = calcPan(channel.pan);
      float lpangain = std::cos((pan + 1.f) * M_PI_4);
      float rpangain = std::sin((pan + 1.f) * M_PI_4);
      mixbuffer[(mix_pos * 2)] += (chunk.data[channel.pos] * gain) * lpangain;
      mixbuffer[(mix_pos * 2) + 1] += (chunk.data[channel.pos] * gain) * rpangain;
      to_mix -= 1;
      mix_pos += 1;
      channel.pos += 1;
      if (!channel.looping && channel.pos >= chunk.length) {
        channel.id = 0;
        channel.active = false;
        chunk.active -= 1;
        break;
      } else {
        channel.pos %= chunk.length;
      }
    }
  }

  for (auto& s : mixbuffer) {
    s = SDL_clamp(s, -1, 1);
  }

  SDL_PutAudioStreamData(stream, mixbuffer.data(), additional_amount);

#ifdef NEED_INTEL_DENORMAL_BIT
  if ((fpustate & 1) == 0) {
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
  }
  if ((fpustate & 2) == 0) {
    _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_OFF);
  }
#endif

}

static void minimp3_callback(void* userdata, SDL_AudioStream* stream, int additional_amount, int total_amount) {
  INT16 mp3pcm[MINIMP3_MAX_SAMPLES_PER_FRAME] {};
  int samples = 0;
  SDL_AudioSpec streamspec {};
  SDL_GetAudioStreamFormat(stream, &streamspec, nullptr);
  int add_samples = additional_amount / streamspec.channels / SDL_AUDIO_BYTESIZE(streamspec.format);
  while (add_samples > 0) {
    if (music_pos >= music_file.size) {
      music_pos = 0;
    }
    mp3dec_frame_info_t info {};
    samples = mp3dec_decode_frame(&music_decoder, ((const UINT8*)(music_file.data)) + music_pos, music_file.size - music_pos, mp3pcm, &info);
    add_samples -= samples;

    SDL_AudioSpec srcspec {};
    srcspec.channels = info.channels;
    srcspec.format = SDL_AUDIO_S16;
    srcspec.freq = info.hz;
    music_pos += info.frame_bytes;

    SDL_AudioSpec dstspec {};
    dstspec.channels = 2;
    dstspec.format = SDL_AUDIO_F32;
    dstspec.freq = 44100;

    if (samples >= 0) {
      SDL_SetAudioStreamFormat(stream, &srcspec, &dstspec);
      SDL_PutAudioStreamData(stream, &mp3pcm, samples * 2 * info.channels);
    } else if (samples == 0) {
      if (info.frame_bytes == 0) {
        music_pos = 0;
      }
    }
  }
}

bool Mixer_Init() {
  chunk_map = {};
  sound_channels = {};
  mixbuffer = {};
  chunk_counter = 1;
  music_active = false;
  if (music_file.valid) {
    close_mapped_file(music_file);
  }

  SDL_InitSubSystem(SDL_INIT_AUDIO);

  SDL_AudioSpec spec {};
  spec.channels = 2;
  spec.format = SDL_AUDIO_F32;
  spec.freq = 44100;
  output_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, audio_callback, nullptr);
  if (!output_stream) {
    return false;
  }
  SDL_ResumeAudioStreamDevice(output_stream);

  return true;
}

void Mixer_Quit() {
  SDL_DestroyAudioStream(output_stream);
  SDL_QuitSubSystem(SDL_INIT_AUDIO);

  chunk_map = {};
  sound_channels = {};
  mixbuffer = {};
}

// I don't want to mess with Caudio's internals for preservation's sake.
// So Caudio just calls into a static mixer module here.

Caudio::Caudio(void) {}
Caudio::~Caudio(void) {}
void Caudio::reset_audio() {
  SDL_LockAudioStream(output_stream);

  chunk_map = {};
  sound_channels = {};
  mixbuffer = {};
  chunk_counter = 1;
  music_active = false;
  if (music_file.valid) {
    close_mapped_file(music_file);
  }

  SDL_UnlockAudioStream(output_stream);
}
void Caudio::checkVolume() {}

static const char* music_trackfiles[] = {
  "",
  "",
  "title.mp3",
  "world1.mp3",
  "world2.mp3",
  "world3.mp3",
  "world4.mp3",
  "gameover.mp3",
};

static float music_trackgains[] = {
  1,
  1,
  .8f,
  .5f,
  .5f,
  .5f,
  .5f,
  1
};
UINT16 Caudio::play_cd(UINT16 tracknr) {
  if (tracknr < 2 || tracknr > 7) {
    return 0;
  }

  SDL_LockAudioStream(output_stream);
  if (music_file.valid) {
    close_mapped_file(music_file);
  }
  std::string trackpath = std::string{"mp3/"} + music_trackfiles[tracknr];
  music_file = map_file(FullPath((char*)trackpath.c_str()));
  if (!music_file.valid) {
    SDL_UnlockAudioStream(output_stream);
    return 0;
  }
  music_gain = music_trackgains[tracknr];
  music_pos = 0;
  mp3dec_init(&music_decoder);
  music_active = true;
  SDL_AudioSpec srcspec {};
  srcspec.channels = 2;
  srcspec.format = SDL_AUDIO_S16;
  srcspec.freq = 44100;
  SDL_AudioSpec dstspec {};
  dstspec.channels = 2;
  dstspec.format = SDL_AUDIO_F32;
  dstspec.freq = 44100;
  music_stream = SDL_CreateAudioStream(&srcspec, &dstspec);
  SDL_SetAudioStreamGetCallback(music_stream, minimp3_callback, nullptr);
  SDL_SetAudioStreamGain(music_stream, music_gain);
  SDL_UnlockAudioStream(output_stream);
}
void Caudio::play_cd_cb(UINT16 tracknr) {
  play_cd(tracknr);
}
void Caudio::stop_cd() {
  SDL_LockAudioStream(output_stream);
  if (music_active) {
    music_active = false;
    if (music_file.valid) {
      close_mapped_file(music_file);
    }
    SDL_DestroyAudioStream(music_stream);
    music_stream = nullptr;
  }
  SDL_UnlockAudioStream(output_stream);
}
HSNDOBJ Caudio::create_sound(int SoundID, int nrof_simult) {
  SDL_AudioSpec spec {};
  Uint8* buf;
  Uint32 len;
  const char *fname = soundFilenames[SoundID];
  std::string appendedpath = std::string{"audio/"} + fname;
  const char *fpath = FullPath((char*)appendedpath.c_str());
  SDL_LoadWAV(fpath, &spec, &buf, &len);

  SDL_AudioSpec dst_spec {};
  dst_spec.channels = 1;
  dst_spec.format = SDL_AUDIO_F32;
  dst_spec.freq = 44100;
  Uint8* cvtbuf;
  int srclen = len;
  int cvtlen;
  SDL_ConvertAudioSamples(&spec, buf, srclen, &dst_spec, &cvtbuf, &cvtlen);
  SDL_free(buf);

  if (!buf) {
    fprintf(stderr, "%s\n", SDL_GetError());
    return 0;
  }

  SDL_LockAudioStream(output_stream);
  HSNDOBJ new_id = chunk_counter++;
  SoundChunk chunk {};
  chunk.poly = nrof_simult;
  chunk.data.resize(cvtlen / 4);
  chunk.length = cvtlen / 4;
  memcpy(chunk.data.data(), cvtbuf, cvtlen);
  chunk_map[new_id] = std::move(chunk);
  SDL_UnlockAudioStream(output_stream);
  SDL_free(cvtbuf);
  return new_id;
}
void Caudio::destroy_sound(HSNDOBJ sound) {
  SDL_LockAudioStream(output_stream);
  for (auto& channel : sound_channels) {
    if (channel.active && channel.id == sound) {
      channel.active = false;
    }
  }
  if (chunk_map.find(sound) != chunk_map.end()) {
    chunk_map.erase(sound);
  }
  SDL_UnlockAudioStream(output_stream);
}
static void play_sound(HSNDOBJ sound, INT32 volume, INT32 pan, bool looping) {
  SDL_LockAudioStream(output_stream);
  SoundChunk& chunk = chunk_map[sound];
  int num_active = 0;
  for (int i = 0; i < sound_channels.size(); i++) {
    SoundChannel& channel = sound_channels[i];
    if (channel.active && channel.id == sound) {
      num_active += 1;
    }
    if (num_active >= chunk.poly) {
      // cancel earlier playbacks?
      SDL_UnlockAudioStream(output_stream);
      return;
    }
  }
  for (int i = 0; i < sound_channels.size(); i++) {
    SoundChannel& channel = sound_channels[i];
    if (!channel.active) {
      channel.active = true;
      channel.looping = looping;
      channel.id = sound;
      channel.vol = volume;
      channel.pan = pan;
      channel.pos = 0;
      chunk.active += 1;
      break;
    }
  }
  SDL_UnlockAudioStream(output_stream);
}
void Caudio::play_sound_1shot(HSNDOBJ sound, INT32 volume, INT32 pan) {
  play_sound(sound, volume, pan, false);
}
void Caudio::play_sound_loop (HSNDOBJ sound, INT32 volume, INT32 pan) {
  play_sound(sound, volume, pan, true);
}
void Caudio::stop_sound(HSNDOBJ sound) {
  SDL_LockAudioStream(output_stream);
  for (auto& channel : sound_channels) {
    if (channel.id == sound) {
      channel.active = false;
      chunk_map[channel.id].active -= 1;
    }
  }
  SDL_UnlockAudioStream(output_stream);
}
void Caudio::stop_cursound(HSNDOBJ sound) {
  stop_sound(sound);
}
void Caudio::sound_volume(HSNDOBJ sound, INT32 volume) {
  SDL_LockAudioStream(output_stream);
  for (auto& channel : sound_channels) {
    if (channel.id == sound) {
      channel.vol = volume;
    }
  }
  SDL_UnlockAudioStream(output_stream);
}
void Caudio::sound_pan(HSNDOBJ sound, INT32 pan) {
  SDL_LockAudioStream(output_stream);
  for (auto& channel : sound_channels) {
    if (channel.id == sound) {
      channel.pan = pan;
    }
  }
  SDL_UnlockAudioStream(output_stream);
}
int	Caudio::get_dsound(void) { return output_stream != nullptr; }
void Caudio::dump(FILE* fd) {}
