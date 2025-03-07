#include "physics_system.hpp"
#include "ui_system.hpp"

PhysicsSystem::PhysicsSystem(entt::registry& reg):
	registry(reg)
{
}




void PhysicsSystem::updatePlayerVelocity(InputState i) {
    auto& motion = registry.get<Motion>(registry.view<Player>().front());
	vec2 proposedVelocity = { 0.0f, 0.0f };
    proposedVelocity.y = (!i.up) ? (i.down ? PLAYER_SPEED : 0.0f) : -PLAYER_SPEED;
    proposedVelocity.x = (!i.left) ? (i.right ? PLAYER_SPEED : 0.0f) : -PLAYER_SPEED;

    if (i.up && i.down)  proposedVelocity.y = 0.0f;
    else if (i.left && i.right) proposedVelocity.x = 0.0f;
    else if (i.left && i.up)    proposedVelocity = PLAYER_SPEED * vec2(-0.7071f, -0.7071f);
    else if (i.left && i.down)  proposedVelocity = PLAYER_SPEED * vec2(-0.7071f, 0.7071f);
    else if (i.right && i.up)    proposedVelocity = PLAYER_SPEED * vec2(0.7071f, -0.7071f);
    else if (i.right && i.down)  proposedVelocity = PLAYER_SPEED * vec2(0.7071f, 0.7071f);

	if (glm::length(proposedVelocity) <= PLAYER_SPEED) {
		motion.velocity = proposedVelocity;
	} 

	
}

// dash  

void PhysicsSystem::step(float elapsed_ms) {
    float elapsed_s = elapsed_ms / 1000;
    updateVelocity(elapsed_s);
    stepAcceleration(elapsed_s); 
}

// always bring it back to 0, maybe change the way it reverts to 0. 
void PhysicsSystem::stepAcceleration(float elapsed_s) {
    auto view = registry.view<Motion>();
    for (auto entity : view) {
        auto& motion = view.get<Motion>(entity);

        float dampingCoefficient = 2.0f; 
        float dampingFactor = exp(-elapsed_s * dampingCoefficient);
        motion.acceleration *= dampingFactor;

        // if magnitude is small enough 0 it out
        if (glm::length(motion.acceleration) < 2.f) {
            motion.acceleration = { 0.0f, 0.0f };
        }
    }
}


void PhysicsSystem::updateVelocity(float elapsed_s) {
    auto players = registry.view<Motion>();
    for (auto entity : players) {
        auto& motion = registry.get<Motion>(entity);
        motion.formerPosition = motion.position;
        motion.velocity += motion.acceleration; // acceleration change
        if (registry.all_of<MarkedCollision>(entity)) {
			motion.velocity = registry.get<MarkedCollision>(entity).velocity;
        } 
        motion.position += motion.velocity * elapsed_s;
        if (registry.all_of<Player>(entity)) {
            UISystem::equipItem(registry, motion);
        }
        if (registry.all_of<Mob>(entity)) {
            UISystem::updateMobHealthBar(registry, entity, false);
        }
    }
}

void PhysicsSystem::ricochet(vec2& velocity, const vec2& normal) {
    float dotProduct = dot(velocity, normal);
   
    velocity = velocity - 2.0f * dotProduct * normal;
    std::cout << velocity.x << " " << velocity.y << std::endl;
}


// Should move both away. 
void PhysicsSystem::suppress(entt::entity& e1, entt::entity& e2) {
    Motion& m1 = registry.get<Motion>(e1); 
    Motion& m2 = registry.get<Motion>(e2);
    vec2 direction = getDirection(e1, e2); // gets e2 to e1 
    float repellentMagnitude = 0.15f; 
    m1.acceleration += direction * repellentMagnitude; 
}

// knocks back e1 in respect to e2's position
void PhysicsSystem::knockback(entt::entity& e1, entt::entity& e2, float force) {
   // std::cout << "SUPPRESSED" << std::endl;
    Motion& m1 = registry.get<Motion>(e1);
    vec2 direction = normalize(getDirection(e1, e2));
    m1.acceleration += direction * force;
}

//NOT NORMALIZED BUT WILL RETURN -1, 0 IF 0,0. 
vec2 PhysicsSystem::getDirection(entt::entity e1, entt::entity e2) {
    Motion& m1 = registry.get<Motion>(e1);
    Motion& m2 = registry.get<Motion>(e2);
    vec2 direction = (m1.position + m1.offset_to_ground) - (m2.position + m2.offset_to_ground);

    if (m1.position - m2.position == vec2(0, 0)) { //prevents undefined when dividing by 0
        direction = vec2(-1, 0); // move to the left, shouldn't happen since acceleration should increase as they get closer. 
    }
    return direction; 
}
