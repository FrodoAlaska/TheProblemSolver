#pragma once

#include "audio/sound_type.h"
#include "audio/music_type.h"
#include "defines.h"

#include <string>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
bool audio_system_init();
void audio_system_shutdown();

void audio_system_add_sound(const SoundType sound, const std::string& path);
void audio_system_add_music(const MusicType music, const std::string& path);

void audio_system_play(const SoundType sound, const f32 pitch);
void audio_system_play(const MusicType music, const f32 pitch, const bool loop = true);
void audio_system_stop(const SoundType sound);
void audio_system_stop(const MusicType music);

void audio_system_set_volume(const f32 sound_volume, const f32 music_volume);
/////////////////////////////////////////////////////////////////////////////////
