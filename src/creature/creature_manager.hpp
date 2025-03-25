#pragma once
#include "creature_common.hpp"
#include "boss_def.hpp"
#include "creature_definition_data.hpp"

class CreatureManager {
public:
    // Retrieve the singleton instance.
    static CreatureManager& getInstance();

    // Load creature definitions (initially hardcoded; can be extended to load from a file).
    void loadDefinitions();

    // Get a creature definition by ID.
    const CreatureDefinitionData* getDefinition(const CreatureID& id) const;

    void registerDefinition(const CreatureDefinitionData* def);

    std::vector<const CreatureDefinitionData*> queryDefinitions(
        const std::function<bool(const CreatureDefinitionData&)>& predicate) const;

    // TODO: hide this in future and provide a getter and setter
    std::vector<BossSpawn> bossSpawnData;

private:
    CreatureManager();
    CreatureManager(const CreatureManager&) = delete;
    CreatureManager& operator=(const CreatureManager&) = delete;

    void loadBossSpawnData();

    std::unordered_map<CreatureID, const CreatureDefinitionData*> definitions;
};