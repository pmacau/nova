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
    loadDefinitions();
    loadBossSpawnData();
}


void CreatureManager::loadDefinitions() {
    // TDOD: Load from file
    definitions.clear();

    std::vector<CreatureDefinition> enemyDefinitions;

    initializeEnemyDefinitions(enemyDefinitions);

    for (const auto& def : enemyDefinitions) {
        definitions[def.id] = def;
    }
}

void CreatureManager::loadBossSpawnData() {
    bossSpawnData.clear();
    initBossSpawnData(bossSpawnData);
}


const CreatureDefinition* CreatureManager::getDefinition(const std::string& id) const {
    auto it = definitions.find(id);
    if (it != definitions.end()) {
        return &it->second;
    }
    std::cerr << "CreatureManager: Unknown creature id: " << id << "\n";
    return nullptr;
}

std::vector<const CreatureDefinition*> CreatureManager::queryDefinitions(
    const std::function<bool(const CreatureDefinition&)>& predicate) const {
    std::vector<const CreatureDefinition*> results;
    for (const auto& kv : definitions) {
        if (predicate(kv.second)) {
            results.push_back(&kv.second);
        }
    }
    return results;
}
