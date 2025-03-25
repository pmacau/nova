#include "creature_manager.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <tinyECS/components.hpp>
#include "enemy_definition.hpp"
#include "boss_def.hpp"

CreatureManager& CreatureManager::getInstance() {
    static CreatureManager instance;
    return instance;
}

CreatureManager::CreatureManager() {
    // loadDefinitions();
    loadBossSpawnData();
}

void CreatureManager::registerDefinition(const CreatureDefinitionData* def) {
    CreatureID id = def->getDefinition().id;
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


void CreatureManager::loadBossSpawnData() {
    bossSpawnData.clear();
    initBossSpawnData(bossSpawnData);
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
