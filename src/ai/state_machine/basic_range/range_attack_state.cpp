#include "range_attack_state.hpp"

#include <animation/animation_component.hpp>
#include <animation_system.hpp>
#include <animation/animation_manager.hpp>
#include <animation/animation_definition.hpp>

#include <world_init.hpp>

#include "music_system.hpp"
#include <util/debug.hpp>

void RangeAttackState::onEnter(entt::registry &registry, entt::entity entity)
{

    if (registry.any_of<AnimationComponent>(entity)) {
        auto& animComp = registry.get<AnimationComponent>(entity);
        AnimationSystem::setAnimationAction(animComp, MotionAction::ATTACK);
    }

    auto &aiComp = registry.get<AIComponent>(entity);
    // Assume the AI config is of type RangeAIConfig.
    const RangeAIConfig &config = static_cast<const RangeAIConfig &>(aiComp.stateMachine->getConfig());

    // randomly select the number of shots to fire
    ivec2 shotsRange = config.shotsNumberRange;

    // std::cout << "RangeAttackState: shotsRange: " << shotsRange.x << ", " << shotsRange.y << "\n";

    // picks a random number between shotsRange.x and shotsRange.y
    rng.seed(std::random_device()());
    std::uniform_int_distribution<int> dist(shotsRange.x, shotsRange.y);
    shotsRemaining = dist(rng);

    shotTimer = 0.0f;
    stateComplete = false;

    debug_printf(DebugType::AI, "Entering RangeAttackState\n");
}

void RangeAttackState::onUpdate(entt::registry &registry, entt::entity entity, float deltaTime)
{
    if (stateComplete)
    {
        std::cout << "RangeAttackState: State already complete, skipping update.\n";
        return;
    }

    auto &aiComp = registry.get<AIComponent>(entity);
    const RangeAIConfig &config = static_cast<const RangeAIConfig &>(aiComp.stateMachine->getConfig());

    shotTimer += deltaTime;
    if (shotTimer >= config.shotCooldown)
    {
        shotTimer = 0.0f;

        shootProjectile(registry, entity, config);
        
        shotsRemaining--;
        // std::cout << "RangeAttackState: Shot fired, remaining: " << shotsRemaining << "\n";
    }

    // Instead of directly calling changeState, we set a flag when done.
    if (shotsRemaining <= 0)
    {
        stateComplete = true;
    }
    // Transition to a new state will be handled via the transition table, which checks isStateComplete().
}

void RangeAttackState::onExit(entt::registry &registry, entt::entity entity)
{
    if (registry.any_of<AIComponent>(entity)) {
        auto& aiComp = registry.get<AIComponent>(entity);
        aiComp.attackCooldownTimer = 0.0f;
    }

    debug_printf(DebugType::AI, "Exiting RangeAttackState\n");
}

void RangeAttackState::shootProjectile(entt::registry &registry, entt::entity entity, const RangeAIConfig &config)
{

    auto playerView = registry.view<Player, Motion>();
    if (playerView.size_hint() == 0)
        return;
    auto playerEntity = *playerView.begin();
    auto &playerMotion = registry.get<Motion>(playerEntity);
    vec2 playerFootPos = playerMotion.position + playerMotion.offset_to_ground;
    auto &motion = registry.get<Motion>(entity);
    vec2 footPos = motion.position + motion.offset_to_ground;
    vec2 diff = playerFootPos - footPos;
    vec2 direction = normalize(diff);

    createProjectile(
            registry,
            motion.position,
            config.projectileSize,
            direction * config.projectileSpeed,
            1,
            PROJECTILE_TIMER,
            config.projectileType,
            {ColliderType::PLAYER});
    MusicSystem::playSoundEffect(SFX::BOW_RELEASE);
}