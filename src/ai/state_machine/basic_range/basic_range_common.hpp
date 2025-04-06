#include <common.hpp>
#include <tinyECS/components.hpp>

#include <ai/ai_common.hpp>

struct RangeAIConfig : public AIConfig {
    ivec2 shotsNumberRange =  {2, 4};
    float shotCooldown = 300.0f;
    float projectileSpeed = 500.0f;

    TEXTURE_ASSET_ID projectileType = TEXTURE_ASSET_ID::WOOD_ARROW;
    vec2 projectileSize = { 64.f, 64.f };
};