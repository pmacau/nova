#pragma once

#include "common.hpp"
#include <entt.hpp>
#include "tinyECS/components.hpp"

class FlagSystem {
public:
    //moved, shot, mobkilled from listening to register, accessed in screenstate (ask Frank)? 
    enum class TutorialStep {
        None, 
        Moved,
        Accessed,
        Shot,
        MobKilled, 
        
    };
    bool is_paused;
private:
    bool done; 
    entt::registry& registry;
    TutorialStep tutorial_step;

public:
    FlagSystem(entt::registry& reg)
        : is_paused(false)
        , done(false)
        , tutorial_step(TutorialStep::None) 
        , registry(reg)
    {
    }

    void step() {
        //if () screenstate no longer
        // set unpaused to false. 
        auto screen_entity = registry.view<ScreenState>().front(); 
        auto& screen_state = registry.get<ScreenState>(screen_entity);
        // if (screen_state.)
        if (done) {
            return; 
        }
        if (tutorial_step == TutorialStep::None) {
            auto view = registry.view<Motion, Player>();
            for (auto entity : view) {
                auto& motion = view.get<Motion>(entity);
                if (motion.velocity.x != 0.0f || motion.velocity.y != 0.0f) {
                    setMoved(true);
                    break;
                }
            }
        }
        else if (tutorial_step == TutorialStep::Moved) {
            auto view = registry.view<ScreenState>();
            setAccessed(true); // TODO: REMOVE THIS WHEN YOU CHANGE BELOW THIS IS JUST FOR PASS THROUGH
            is_paused = true; //TODO: Move below 
            for (auto entity : view) {
                //TODO:: Frank just change the if statement to whatever state gets changed in registry and it should work. 
                //auto& screen = registry.get<ScreenState>(entity);
                //if (screen.darken_screen_factor > 0.0f) {  
                //    setAccessed(true);
                //    break;
                //}
            }
        }
        else if (tutorial_step == TutorialStep::Accessed) {
            auto proj_view = registry.view<Projectile>();
            if (!proj_view.empty()) {
                setShot(true); // just leave it at Shot and then display the text. 
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
    TutorialStep getTutorialStep() const { return tutorial_step; }


    void setMobKilled(bool value) {
        if (value && tutorial_step == TutorialStep::Shot) {
            tutorial_step = TutorialStep::MobKilled;
            debug_printf(FLAG, "setMobKilled: stepped from accessed to mobKilled\n");
        }
    }

    
    void setMoved(bool value) {
        if (value && tutorial_step == TutorialStep::None) {
            tutorial_step = TutorialStep::Moved;
            debug_printf(FLAG, "setMoved: stepped from none to moved\n");
        }
    }

    void setAccessed(bool value) {
        if (value && tutorial_step == TutorialStep::Moved) {
            tutorial_step = TutorialStep::Accessed;
            debug_printf(FLAG, "setAccessed: stepped from moved to accessed\n");
        }
    }

   
    void setShot(bool value) {
        if (value && tutorial_step == TutorialStep::Accessed) {
            tutorial_step = TutorialStep::Shot;
            debug_printf(FLAG, "setShot: stepped from accessed to shot\n");
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