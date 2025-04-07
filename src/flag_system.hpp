#pragma once

#include "common.hpp"
#include <entt.hpp>
#include "tinyECS/components.hpp"
#include "util/debug.hpp"
class FlagSystem {
public:
    //moved, shot, mobkilled from listening to register, accessed in screenstate (ask Frank)? 
    enum class TutorialStep {
        None, 
        Moved,
        Accessed,
        Shot,
        Melee_Dash,
        Biome_Read,
        MobKilled,
        Done
    };
    bool iceKilled; 
    bool jungleKilled; 
    bool savanaKilled; 
	bool forestKilled;
    bool is_paused;
    float time_spent_s; 
private:
    entt::registry& registry;
    TutorialStep tutorial_step;

    bool used_inventory = false;

public:
    FlagSystem(entt::registry& reg)
        : is_paused(false)
        , time_spent_s(0)
        , registry(reg)
        , tutorial_step(TutorialStep::None)
    {
    }

    void step(float elapsed_ms) {

        //if () screenstate no longer
        // set unpaused to false. 

        if (tutorial_step != TutorialStep::Moved) {
            auto view = registry.view<ScreenState>();
            for (auto entity : view) {
                auto& screen_state = registry.get<ScreenState>(entity);
                if (screen_state.current_screen == ScreenState::ScreenType::END_SCREEN ||
                    screen_state.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI ||
                    screen_state.current_screen == ScreenState::ScreenType::UPGRADE_UI ||
                    screen_state.current_screen == ScreenState::ScreenType::TITLE) {
                    is_paused = true;
                    return;
                }
            }
        }
        is_paused = false;

        if (tutorial_step == TutorialStep::Done) return;

        if (!is_paused) {
            time_spent_s += (elapsed_ms / 1000.f);
        }
       
        if (tutorial_step == TutorialStep::Moved) {
            auto view = registry.view<ScreenState>();
            for (auto entity : view) { 
                auto& screen = registry.get<ScreenState>(entity);
                if (screen.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI ||
                    screen.current_screen == ScreenState::ScreenType::UPGRADE_UI
                ) { 
                    is_paused = true; 
                    setAccessed(true);
                    break;
                }
                if (
                    screen.current_screen == ScreenState::ScreenType::TITLE ||
                    screen.current_screen == ScreenState::ScreenType::END_SCREEN
                ) {
                    is_paused = true;
                }
            }
        }
    }
  
    bool getIsPaused() const { return is_paused; }
    bool isDone() const {
        return (
            tutorial_step == TutorialStep::Done ||
            tutorial_step == TutorialStep::Shot ||
            tutorial_step == TutorialStep::Biome_Read ||
            tutorial_step == TutorialStep::MobKilled
        );
    };

    TutorialStep getTutorialStep() const { return tutorial_step; }

    void setMobKilled(bool value) {
        if (value && tutorial_step == TutorialStep::Done && !used_inventory) {
            tutorial_step = TutorialStep::MobKilled;
            time_spent_s = 0;
            used_inventory = true;
            debug_printf(DebugType::FLAG, "setMobKilled: stepped from accessed to mobKilled\n");
        }
    }
    
    void setMoved(bool value) {
        if (value && tutorial_step == TutorialStep::None) {
            tutorial_step = TutorialStep::Moved;
            time_spent_s = 0;
            debug_printf(DebugType::FLAG, "setMoved: stepped from none to moved\n");
        }
    }

    void setAccessed(bool value) {
        if (value && tutorial_step == TutorialStep::Moved) {
            tutorial_step = TutorialStep::Accessed;
            time_spent_s = 0;
            debug_printf(DebugType::FLAG, "setAccessed: stepped from moved to accessed\n");
        }
    }

   
    void setShot(bool value) {
        if (value && tutorial_step == TutorialStep::Accessed) {
            tutorial_step = TutorialStep::Shot;
            time_spent_s = 0;
            debug_printf(DebugType::FLAG, "setShot: stepped from accessed to shot\n");
        }
    }

    void setMeleeDash(bool value) {
        if (value && tutorial_step == TutorialStep::Shot) {
            tutorial_step = TutorialStep::Melee_Dash;
            time_spent_s = 0;
            debug_printf(DebugType::FLAG, "setMeleeDash: stepped from shot to melee-dash\n");
        }
    }

    void setBiomeRead(bool value) {
        if (value && tutorial_step == TutorialStep::Melee_Dash) {
            tutorial_step = TutorialStep::Biome_Read;
            time_spent_s = 0;
            debug_printf(DebugType::FLAG, "setBiomeRead: stepped from Melee to biomeRead\n");
        }
    }

    void setDone(bool value) {
        if (value) {
            tutorial_step = TutorialStep::Done;
            time_spent_s = 0;
            debug_printf(DebugType::FLAG, "setDone: stepped from biomeRead to done\n");
        }
    }
    
    void setIsPaused(bool value) {
        is_paused = value;
    }

    // resets everything
    void reset() {
        bool iceKilled = false;
        bool jungleKilled = false;
        bool savanaKilled = false;
        bool forestKilled = false;
        is_paused = false;
        //tutorial_step = TutorialStep::None;
    }
};