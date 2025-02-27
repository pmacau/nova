#pragma once
#include "common.hpp"

#include <vector>
#include <unordered_map>
#include <string>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>


template <typename T>
struct SoundData {
    const std::string filename;
    T* sound = nullptr;
};

enum SFX {
    SHOOT, HIT, POTION, EQUIP, PICKUP,
    SFX_COUNT
};
const int sfx_count = (int) SFX_COUNT;

enum Music {
    FOREST,
    MUSIC_COUNT
};
const int music_count = (int) MUSIC_COUNT;


class MusicSystem {
public:
        static bool init() {
            return (
                SDL_Init(SDL_INIT_AUDIO) < 0
                || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1
                || !load_sounds<SFX, Mix_Chunk>(
                    sfx_map, [](const char* name) -> Mix_Chunk* {return Mix_LoadWAV(name);}
                )
                || !load_sounds<Music, Mix_Music>(music_map, Mix_LoadMUS)
            );
        }

        static void clear() {
            clear_sounds<SFX, Mix_Chunk>(sfx_map, Mix_FreeChunk);
            clear_sounds<Music, Mix_Music>(music_map, Mix_FreeMusic);            
            Mix_CloseAudio();
        }

		static void playSoundEffect(SFX effect, int loops = 0, int channel = -1) {
            if (sfx_map.find(effect) != sfx_map.end()) {
                SoundData<Mix_Chunk>& data = sfx_map[effect];
                Mix_PlayChannel(channel, data.sound, loops);
            };
        }

        static void playMusic(Music music, int loops = -1) {
            if (music_map.find(music) != music_map.end()) {
                SoundData<Mix_Music>& data = music_map[music];
                Mix_PlayMusic(data.sound, loops);
            };
        }
private:
        inline static std::unordered_map<SFX, SoundData<Mix_Chunk>> sfx_map = {
            {SHOOT,  {"sfx/shoot.wav"}},
            {HIT,    {"sfx/hit.wav"}},
            {POTION, {"sfx/potion.wav"}},
            {EQUIP,  {"sfx/equip.wav"}},
            {PICKUP, {"sfx/pickup.wav"}}
        };
        inline static std::unordered_map<Music, SoundData<Mix_Music>> music_map = {
            {FOREST, {"forest.wav"}}
        };

        template <typename EnumT, typename SoundT>
        static bool load_sounds(
            std::unordered_map<EnumT, SoundData<SoundT>>& audio_map,
            const std::function<SoundT*(const char*)> load_func
        ) {
            for (auto& [key, val] : audio_map) {
                val.sound = load_func(audio_path(val.filename).c_str());
                if (val.sound == nullptr) {
                    fprintf(stderr, "Failed to load sound: %s\n", val.filename.c_str());
                    return false;
                }
            }
            return true;  
        };

        template <typename EnumT, typename SoundT>
        static void clear_sounds(
            std::unordered_map<EnumT, SoundData<SoundT>>& audio_map,
            const std::function<void(SoundT*)>& clear_func
        ) {
            for (auto& [key, val] : audio_map) {
                if (val.sound != nullptr) clear_func(val.sound);
            }
        }
};