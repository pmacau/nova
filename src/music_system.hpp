#pragma once
#include "common.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>
#include <thread>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>


template <typename T>
struct SoundData {
    const std::string filename;
    T* sound = nullptr;
};

enum SFX {
    SHOOT, HIT, POTION, EQUIP, PICKUP, WOOD, SELECT,
    SFX_COUNT
};
const int sfx_count = (int) SFX_COUNT;

enum Music {
    FOREST, BEACH, JUNGLE, OCEAN, SAVANNA, SNOWLANDS,
    MUSIC_COUNT
};
const int music_count = (int) MUSIC_COUNT;


class MusicSystem {
public:
        static bool init() {
            return (
                SDL_Init(SDL_INIT_AUDIO) == 0 &&
                Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == 0 &&
                load_sounds<SFX, Mix_Chunk>(
                    sfx_map, [](const char* name) -> Mix_Chunk* {return Mix_LoadWAV(name);}
                ) &&
                load_sounds<Music, Mix_Music>(music_map, Mix_LoadMUS)
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

        static void playMusic(Music music, int loops = -1, int fade_time = 0) {
            if (music != currentTrack && music_map.find(music) != music_map.end()) {
                std::thread(fade_to_track, music, loops, fade_time).detach();
            };
        }
private:
        inline static std::unordered_map<SFX, SoundData<Mix_Chunk>> sfx_map = {
            {SHOOT,  {"sfx/shoot.wav"}},
            {HIT,    {"sfx/hit.wav"}},
            {POTION, {"sfx/potion.wav"}},
            {EQUIP,  {"sfx/equip.wav"}},
            {PICKUP, {"sfx/pickup.wav"}},
            {WOOD,   {"sfx/wood.wav"}},
            {SELECT, {"sfx/select.wav"}}
        };
        inline static std::unordered_map<Music, SoundData<Mix_Music>> music_map = {
            {FOREST,    {"music/forest.wav"}},
            {BEACH,     {"music/beach.wav"}},
            {JUNGLE,    {"music/jungle.wav"}},
            {OCEAN,     {"music/ocean.wav"}},
            {SAVANNA,   {"music/savanna.wav"}},
            {SNOWLANDS, {"music/snowlands.wav"}}
        };

        template <typename EnumT, typename SoundT>
        static bool load_sounds(
            std::unordered_map<EnumT, SoundData<SoundT>>& audio_map,
            const std::function<SoundT*(const char*)> load_func
        ) {
            for (auto& [key, val] : audio_map) {
                val.sound = load_func(audio_path(val.filename).c_str());
                if (val.sound == nullptr) {
                    fprintf(
                        stderr, 
                        "Failed to load sound: %s\nError: %s\n",
                        val.filename.c_str(), Mix_GetError()
                    );
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


        inline static std::mutex musicMutex;
        inline static Music currentTrack = MUSIC_COUNT;

        static void fade_to_track(Music music, int loops = -1, int fade_time = 0) {
            std::lock_guard<std::mutex> lock(musicMutex);

            if (Mix_PlayingMusic()) {
                Mix_FadeOutMusic(fade_time);
                std::this_thread::sleep_for(std::chrono::milliseconds(fade_time));
            }

            SoundData<Mix_Music>& data = music_map[music];
            Mix_FadeInMusic(data.sound, loops, fade_time);
            currentTrack = music;
        }
};