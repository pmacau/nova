#pragma once

#include "common.hpp"
#include <entt.hpp>
#include "tinyECS/components.hpp"
#include "util/debug.hpp"
#include "creature/boss_def.hpp"
#include "world_init.hpp"
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
	bool beachKilled;
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
        iceKilled = false;
        jungleKilled = false;
        savanaKilled = false;
        beachKilled = false;
        is_paused = false;
        //tutorial_step = TutorialStep::None;
    }

    void bossDefeatedHelper(CreatureID id) {
        if (id == CreatureID::BOSS) {
            iceKilled = true; 
			//std::cout << "Ice boss killed" << std::endl;
        }
        else if (id == CreatureID::BOSS_BEACH_RED) {
			savanaKilled = true;
			//std::cout << "Savanna boss killed" << std::endl;
		}
		else if (id == CreatureID::BOSS_FOREST_PURPLE) {
			jungleKilled = true;
            //std::cout << "Jungle boss killed" << std::endl;
		}
		else if (id == CreatureID::BOSS_JUNGLE_YELLOW) {
			beachKilled = true;
            //std::cout << "Beach boss killed" << std::endl;
		}

        vec2 size = vec2(2 * WINDOW_WIDTH_PX / 3, 200);
        int scale = 2;

        std::vector<std::string> remaining;
        if (!iceKilled) remaining.push_back("{1Ice Titan{");
        if (!jungleKilled) remaining.push_back("{1Jungle Behemoth}");
        if (!savanaKilled) remaining.push_back("{1Savanna Colossus}");
        if (!beachKilled) remaining.push_back("{1Siren of the Sands}");

        
        if (!remaining.empty()) {
            std::string message = "Remaining bosses:\n";
            for (auto& boss : remaining) {
                message += " - " + boss + "\n";
            }
            message += "\nDefeat all to {1upgrade} your ship!";

            auto text = createTextBox(registry,
                vec2(0.f, 200.0f), size, message, scale, vec3(1)
            );

            registry.emplace<tempText>(text); 

            auto& textData = registry.get<TextData>(text);
			textData.active = true;
		}
		else {
			std::string message = "All bosses {1defeated}! {1Nothing} is holding you back from {1upgrading} the ship now!";
			auto text = createTextBox(registry,
				vec2(0.f, 200.0f), size, message, scale, vec3(1)
			);
			registry.emplace<tempText>(text);
			auto& textData = registry.get<TextData>(text);
    }
};