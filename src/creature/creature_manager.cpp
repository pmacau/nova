#include "creature_manager.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <tinyECS/components.hpp>
#include "boss_def.hpp"

#include "creature/creature_defs/blue_torch_goblin.hpp"
#include "creature/creature_defs/red_torch_goblin.hpp"
#include "creature/creature_defs/purple_torch_goblin.hpp"
#include "creature/creature_defs/yellow_torch_goblin.hpp"
#include "creature/creature_defs/purple_reaper.hpp"

CreatureManager& CreatureManager::getInstance() {
    static CreatureManager instance;
    return instance;
}

CreatureManager::CreatureManager() {
    // loadDefinitions();
    loadDefinitions();
}

void CreatureManager::registerDefinition(const CreatureDefinitionData* def) {
    CreatureID id = def->getCreatureID();
    definitions[id] = def;
}

const CreatureDefinitionData* CreatureManager::getDefinition(const CreatureID& id) const {
    auto it = definitions.find(id);
    if (it != definitions.end()) {
        return it->second;
    }
    std::cerr << "CreatureManager: Unknown creature id: " << static_cast<int>(id) << "\n";
    return nullptr;
}



std::vector<const CreatureDefinitionData*> CreatureManager::queryDefinitions(
    const std::function<bool(const CreatureDefinitionData&)>& predicate) const
{
    std::vector<const CreatureDefinitionData*> results;
    for (const auto& kv : definitions) {
        if (predicate(*kv.second)) {
            results.push_back(kv.second);
        }
    }
    return results;
}

void CreatureManager::loadDefinitions() {
    registerDefinition(&BlueTorchGoblinDefData::getInstance());
    registerDefinition(&RedTorchGoblinDefData::getInstance());
    registerDefinition(&PurpleTorchGoblinDefData::getInstance());
    registerDefinition(&YellowTorchGoblinDefData::getInstance());
    registerDefinition(&ForestPurpleWizardData::getInstance());
    registerDefinition(&BeachPurpleWizardData::getInstance());
    registerDefinition(&JunglePurpleWizardData::getInstance());
    registerDefinition(&DefaultWizardData::getInstance());
}
