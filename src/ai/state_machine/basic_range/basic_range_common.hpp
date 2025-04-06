#include <common.hpp>
#include <ai/ai_common.hpp>

struct RangeAIConfig : public AIConfig {
    ivec2 shotsNumberRange =  {2, 4};
    float shotCooldown = 300.0f;
    float projectileSpeed = 500.0f;
};