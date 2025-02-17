#include "physics_system.hpp"

PhysicsSystem::PhysicsSystem(entt::registry& reg):
	registry(reg)
{
}



void PhysicsSystem::checkMovement() {
    // Movement checking code here
}

//knockback
//hitbox flag refactorig
// debug mode bind to a key 
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
        if (glm::length(motion.acceleration) < 0.02f) {
            motion.acceleration = { 0.0f, 0.0f };
        }
    }
}


void PhysicsSystem::updateVelocity(float elapsed_s) {
    auto players = registry.view<Motion>();
    for (auto entity : players) {
        auto& motion = registry.get<Motion>(entity);
        motion.formerPosition = motion.position; // incase of redirect. 
        motion.velocity += motion.acceleration; // acceleration change
        motion.position += motion.velocity * elapsed_s;
    }
}

// Should move both away. 
void PhysicsSystem::suppress(entt::entity& e1, entt::entity& e2) {
    //std::cout << "SUPPRESSED" << std::endl; 
    Motion& m1 = registry.get<Motion>(e1); 
    Motion& m2 = registry.get<Motion>(e2);
    vec2 direction = getDirection(e1, e2); // gets e2 to e1
    // set accelerations based off positions, if very close then very high repellent. 
    /*std::cout << m1.position.x - m2.position.x << std::endl; 
    std::cout << m1.position.y - m2.position.y << std::endl;*/
    float repellentAccelerationX = min(1 / exp(-abs(m1.position.x - m2.position.x)), 30.f); 
    float repellentAccelerationY = min(1 / exp(-abs(m1.position.y - m2.position.y)), 30.f);
    m1.acceleration.x = repellentAccelerationX; 
    m1.acceleration.y = repellentAccelerationY; 
    m2.acceleration.x = repellentAccelerationX;
    m2.acceleration.y = repellentAccelerationY;

    std::cout << "X: " << repellentAccelerationX << " Y: " << repellentAccelerationY << std::endl;
    


}

vec2 PhysicsSystem::getDirection(entt::entity e1, entt::entity e2) {
    Motion& m1 = registry.get<Motion>(e1);
    Motion& m2 = registry.get<Motion>(e2);
    /*std::cout << "M1" << std::endl;
    std::cout << m1.position.x << std::endl; 
    std::cout << m1.position.y << std::endl;
    std::cout << "M2" << std::endl;
    std::cout << m2.position.x << std::endl;
    std::cout << m2.position.y << std::endl;*/

    vec2 direction = m1.position - m2.position;
    if (m1.position - m2.position == vec2(0, 0)) { //prevents undefined when dividing by 0
        direction = vec2(-1, 0); // move to the left, shouldn't happen since acceleration should increase as they get closer. 
    }
    else {
        direction = normalize(direction);
    }
    return direction; 
}
