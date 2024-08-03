#pragma once

#include "audio/sound_type.h"
#include "audio/music_type.h"

#include <string>

// Public functions
/////////////////////////////////////////////////////////////////////////////////
bool audio_system_init();
void audio_system_shutdown();
void audio_system_add_sound(const SoundType sound, const std::string& path);
void audio_system_add_music(const MusicType music, const std::string& path);
/////////////////////////////////////////////////////////////////////////////////
