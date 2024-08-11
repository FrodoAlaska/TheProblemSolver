#include "audio_system.h"
#include "audio/music_type.h"
#include "audio/sound_type.h"
#include "defines.h"

#include <miniaudio/miniaudio.h>

#include <cstdio>
#include <string>

// AudioSystem
/////////////////////////////////////////////////////////////////////////////////
struct AudioSystem {
  ma_engine* engine;
  ma_sound sounds[SOUNDS_MAX];
  ma_sound music[MUSIC_MAX];

  f32 sound_volume, music_volume;
};

static AudioSystem audio_sys;
/////////////////////////////////////////////////////////////////////////////////

// Private functions 
/////////////////////////////////////////////////////////////////////////////////
static void load_sound(const std::string& path, const SoundType type) {
  ma_result result = ma_sound_init_from_file(audio_sys.engine, path.c_str(), 0, nullptr, nullptr, &audio_sys.sounds[type]);
  if(result != MA_SUCCESS) {
    fprintf(stderr, "[AUDIO-ERROR, ERROR-CODE = %i]: Failed to load audio at \'%s\'\n", result, path.c_str());
    return;
  }
}

static void load_music(const std::string& path, const MusicType type) {
  ma_result result = ma_sound_init_from_file(audio_sys.engine, path.c_str(), 0, nullptr, nullptr, &audio_sys.music[type]);
  if(result != MA_SUCCESS) {
    fprintf(stderr, "[AUDIO-ERROR, ERROR-CODE = %i]: Failed to load audio at \'%s\'\n", result, path.c_str());
    return;
  }
}
/////////////////////////////////////////////////////////////////////////////////

// Public functions
/////////////////////////////////////////////////////////////////////////////////
bool audio_system_init() {
  audio_sys.engine = new ma_engine{};

  ma_result result = ma_engine_init(nullptr, audio_sys.engine);
  if(result != MA_SUCCESS) {
    fprintf(stderr, "[AUDIO-ERROR, ERROR-CODE = %i]: Failed to initialize \'ma_engine\'\n", result);
    delete audio_sys.engine;

    return false;
  }

  audio_sys.sound_volume = 0.3f;
  audio_sys.music_volume = 1.0f;

  return true;
}

void audio_system_shutdown() {
  ma_engine_uninit(audio_sys.engine);
  delete audio_sys.engine;
}

void audio_system_add_sound(const SoundType sound, const std::string& path) {
  load_sound(path, sound);
}

void audio_system_add_music(const MusicType music, const std::string& path) {
  load_music(path, music);
}

void audio_system_play(const SoundType sound, const f32 pitch) {
  ma_sound* snd = &audio_sys.sounds[sound];

  // Stop the sound if it is already playing
  audio_system_stop(sound);

  ma_sound_set_volume(snd, audio_sys.sound_volume);
  ma_sound_set_pitch(snd, pitch);
  ma_sound_start(snd);
}

void audio_system_play(const MusicType music, const f32 pitch, const bool loop) {
  ma_sound* msc = &audio_sys.music[music];
  
  audio_system_stop(music);

  ma_sound_set_looping(msc, loop);
  ma_sound_set_volume(msc, audio_sys.music_volume);
  ma_sound_set_pitch(msc, pitch);
  ma_sound_start(msc);
}

void audio_system_set_volume(const f32 sound_volume, const f32 music_volume) {
  audio_sys.sound_volume = sound_volume;
  audio_sys.music_volume = music_volume;
}

void audio_system_stop(const SoundType sound) {
  ma_sound* snd = &audio_sys.sounds[sound];

  if(ma_sound_is_playing(snd)) {
    ma_sound_stop(snd);
    ma_sound_seek_to_pcm_frame(snd, 0);
  }
}

void audio_system_stop(const MusicType music) {
  ma_sound* snd = &audio_sys.music[music];

  if(ma_sound_is_playing(snd)) {
    ma_sound_stop(snd);
    ma_sound_seek_to_pcm_frame(snd, 0);
  }
}
/////////////////////////////////////////////////////////////////////////////////
