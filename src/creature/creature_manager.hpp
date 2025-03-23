#pragma once
#include "common.hpp"
#include "boss_def.hpp"

class CreatureManager {
public:
    // Retrieve the singleton instance.
    static CreatureManager& getInstance();

    // Load creature definitions (initially hardcoded; can be extended to load from a file).
    void loadDefinitions();

    // Get a creature definition by ID.
    const CreatureDefinition* getDefinition(const std::string& id) const;

    std::vector<const CreatureDefinition*> queryDefinitions(
        const std::function<bool(const CreatureDefinition&)>& predicate) const;

    // TODO: hide this in future and provide a getter and setter
    std::vector<BossSpawn> bossSpawnData;

private:
    CreatureManager();
    CreatureManager(const CreatureManager&) = delete;
    CreatureManager& operator=(const CreatureManager&) = delete;

    void loadBossSpawnData();

    std::unordered_map<std::string, CreatureDefinition> definitions;
};