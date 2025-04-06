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
        Melee,
        Dash, 
        Biome_Read,
        MobKilled,
        Done
    };
    bool is_paused;
    float time_spent_s; 
private:
    entt::registry& registry;
    TutorialStep tutorial_step;

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
        
        
        //debug_printf(DebugType::FLAG, "time is now %f\n", time_spent_s);
        

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
            //time_spent_s = std::min(elapsed_ms / 1000.f + time_spent_s, 10.f);
            time_spent_s += (elapsed_ms / 1000.f);
            //debug_printf(DebugType::FLAG, "time is now %f\n", time_spent_s);
        }
        if (tutorial_step == TutorialStep::None) {
            auto view = registry.view<Motion, Player>();
            for (auto entity : view) {
                auto& motion = view.get<Motion>(entity);
                if ((motion.velocity.x != 0.0f || motion.velocity.y != 0.0f) && time_spent_s > 10.0f) {
                    setMoved(true);
                    break;
                }
            }
        }
        else if (tutorial_step == TutorialStep::Moved) {
            auto view = registry.view<ScreenState>();
            for (auto entity : view) { 
                auto& screen = registry.get<ScreenState>(entity);
                if (screen.current_screen == ScreenState::ScreenType::END_SCREEN ||
                    screen.current_screen == ScreenState::ScreenType::SHIP_UPGRADE_UI ||
                    screen.current_screen == ScreenState::ScreenType::UPGRADE_UI ||
                    screen.current_screen == ScreenState::ScreenType::TITLE) { 
                    is_paused = true; 
                    setAccessed(true);
                    break;
                }
            }
        }
        else if (tutorial_step == TutorialStep::Accessed) {
            auto proj_view = registry.view<Projectile>();
            if (!proj_view.empty()) {
                setShot(true); // just leave it at Shot and then display the text. 
            }
        }
        else if (tutorial_step == TutorialStep::Shot) {
            // timer for 20s to read the prompt
            if (time_spent_s > 4.0f) {
                setMelee(true);
            }
        }
        else if (tutorial_step == TutorialStep::Melee) {
            if (time_spent_s > 4.0f) {
                
                setDash(true);
            }
        }
        else if (tutorial_step == TutorialStep::Dash) {
			auto view = registry.view<Motion, Player>();
            if (time_spent_s > 20.0f) {
                setBiomeRead(true);
            }
        }
        else if (tutorial_step == TutorialStep::Biome_Read) {
            if (time_spent_s > 10.0f) {
                setDone(true);
            }
        }
        /*else if (tutorial_step == TutorialStep::Shot) { Don't really know a good way about going about this, we can leave this out since it should be enough. 
        *  You can leave a message after shooting to tell the player to go explore... 
            auto mob_view = registry.view<Mob>();  
            for (auto entity : mob_view) {
                auto& mob = registry.get<Mob>(entity);
                if (mob.health <= 0) {
                    setMobKilled(true);
                    break;
                }
            }
        }*/
    }

  
    bool getIsPaused() const { return is_paused; }
    bool isDone() const {return tutorial_step == TutorialStep::Done; };

    TutorialStep getTutorialStep() const { return tutorial_step; }

    void setMobKilled(bool value) {
        if (value && tutorial_step == TutorialStep::Shot) {
            tutorial_step = TutorialStep::MobKilled;
            time_spent_s = 0;
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

    void setMelee(bool value) {
        if (value && tutorial_step == TutorialStep::Shot) {
            tutorial_step = TutorialStep::Melee;
            time_spent_s = 0;
            debug_printf(DebugType::FLAG, "setMelee: stepped from shot to melee\n");
        }
    }

    void setDash(bool value) {
        if (value && tutorial_step == TutorialStep::Melee) {
            tutorial_step = TutorialStep::Dash;
            time_spent_s = 0;
            debug_printf(DebugType::FLAG, "setDash: stepped from Melee to Dash\n");
        }
    }

    void setBiomeRead(bool value) {
        if (value && tutorial_step == TutorialStep::Dash) {
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
        is_paused = false;
        tutorial_step = TutorialStep::None;
    }
};