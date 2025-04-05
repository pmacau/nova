#include "physics_system.hpp"
#include "ui_system.hpp"

PhysicsSystem::PhysicsSystem(entt::registry& reg):
	registry(reg)
{
}





void PhysicsSystem::updatePlayerVelocity(InputState i) {
    if (registry.view<Player>().empty()) return;
    auto player_entity = registry.view<Player>().front(); 
	    auto dash = registry.get<Dash>(player_entity);
    if (dash.inUse) {
    return;
    }
    auto& player = registry.get<Player>(player_entity);
    InputState& player_direction = player.direction;
    if (i.up || i.down || i.left || i.right) {
        player_direction.down = false;
        player_direction.up = false;
        player_direction.left = false;
        player_direction.right = false;
    }
    
    

    auto& motion = registry.get<Motion>(registry.view<Player>().front());
	    vec2 proposedVelocity = { 0.0f, 0.0f };
   

    if (i.up) {
        proposedVelocity.y = -PLAYER_SPEED;
        player_direction.up = true;
    }
    else if (i.down){
        proposedVelocity.y = PLAYER_SPEED;
        player_direction.down = true;
    }
    else if (i.right) {
        proposedVelocity.x = PLAYER_SPEED; 
        player_direction.right = true; 
    }
    else if (i.left) {
        proposedVelocity.x = -PLAYER_SPEED;
        player_direction.left = true;
    }

    if (i.up && i.down) {
        proposedVelocity.y = 0.0f;
        player_direction.up = true; 
		player_direction.down = true;
    }
    else if (i.left && i.right) {
        proposedVelocity.x = 0.0f;
        player_direction.right = true; 
        player_direction.left = true;
    }
    else if (i.left && i.up) {
        proposedVelocity = PLAYER_SPEED * vec2(-0.7071f, -0.7071f);
        player_direction.left = true; 
        player_direction.up = true;
    }
    else if (i.left && i.down) {
        proposedVelocity = PLAYER_SPEED * vec2(-0.7071f, 0.7071f);
        player_direction.left = true; 
        player_direction.down = true;
    }
    else if (i.right && i.up) {
        proposedVelocity = PLAYER_SPEED * vec2(0.7071f, -0.7071f);
        player_direction.right = true; 
        player_direction.up = true;
    }
    else if (i.right && i.down) {
        proposedVelocity = PLAYER_SPEED * vec2(0.7071f, 0.7071f);
        player_direction.right = true;
        player_direction.down = true;
    }

    if (glm::length(proposedVelocity) <= PLAYER_SPEED) {
        motion.velocity = proposedVelocity;
    } 	
}


// dash  
void PhysicsSystem::dash() {
    auto player = registry.view<Player, Dash>().front();
    auto& dash = registry.get<Dash>(player);
    if (dash.inUse) {
        return;
    }
    else if (dash.cooldown < 0){
        dash.inUse = true; 
		dash.remainingDuration = 0.15f;
    }
	auto& motion = registry.get<Motion>(player);
	// motion.acceleration = 2.0f * motion.velocity;
    motion.acceleration = { 0, 0 };
    motion.velocity = 1200.f * glm::normalize(motion.velocity);
    
	
}

void PhysicsSystem::step(float elapsed_ms) {
    float elapsed_s = elapsed_ms / 1000;
    updateVelocity(elapsed_s);
    stepAcceleration(elapsed_s); 
    updatePlayerState(elapsed_s); 
}

void PhysicsSystem::updatePlayerState(float elapsed_s) {
	auto player = registry.view<Player, Dash>().front();
	auto& dash = registry.get<Dash>(player);
	if (dash.inUse && dash.remainingDuration > 0.0f) {
		dash.remainingDuration -= elapsed_s;
	} 
	if (dash.cooldown > 0.0f) {
		dash.cooldown -= elapsed_s;
	}
	if (dash.remainingDuration < 0.0f) {
		dash.inUse = false;
		dash.remainingDuration = 0.0f;
		dash.cooldown = 1.5f;
	}

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
        if (glm::length(motion.velocity + motion.acceleration) > MAX_SPEED) {
            motion.velocity = glm::normalize(motion.velocity) * MAX_SPEED;
        }
        else {
            motion.velocity += motion.acceleration; // acceleration change
        }
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
        if (registry.any_of<HomingMissile>(entity)) {
            auto& missile_postion = registry.get<Motion>(entity).position;
            if (!registry.valid(registry.get<HomingMissile>(entity).target)) continue;
            auto& target_entity = registry.get<HomingMissile>(entity).target;
            auto& target_position = registry.get<Motion>(target_entity).position;
            vec2 current_direction = normalize(motion.velocity);
            vec2 mob_direction = normalize(target_position - missile_postion);
            motion.velocity = (current_direction + (mob_direction - current_direction) * elapsed_s * 5.f) * PROJECTILE_SPEED;
            motion.angle = atan2(motion.velocity.y, motion.velocity.x) * (180.0f / M_PI) + 90.0f;
        }
    }
}


// Should move both away. 
void PhysicsSystem::suppress(entt::entity& e1, entt::entity& e2) {
    Motion& m1 = registry.get<Motion>(e1); 
    //Motion& m2 = registry.get<Motion>(e2);
    vec2 direction = getDirection(e1, e2); // gets e2 to e1 
    float repellentMagnitude = 0.15f; 
    m1.acceleration += direction * repellentMagnitude; 
}

// knocks back e1 in respect to e2's position, E1 MUST BE PLAYER
void PhysicsSystem::knockback(entt::entity& e1, entt::entity& e2, float force) {
    /*auto player = registry.view<Player, Dash>().front(); 
    auto dash = registry.get<Dash>(player); 
    if (dash.inUse) {
        return; 
    }*/
    Motion& m1 = registry.get<Motion>(e1);
    vec2 direction = normalize(getDirection(e1, e2));
    m1.acceleration += direction * force;
}

//NOT NORMALIZED BUT WILL RETURN -1, 0 IF 0,0. 
vec2 PhysicsSystem::getDirection(entt::entity e1, entt::entity e2) {
    Motion& m1 = registry.get<Motion>(e1);
    Motion& m2 = registry.get<Motion>(e2);
    vec2 direction = m1.position - m2.position;
    if (m1.position - m2.position == vec2(0, 0)) { //prevents undefined when dividing by 0
        direction = vec2(-1, 0); // move to the left, shouldn't happen since acceleration should increase as they get closer. 
    }
    return direction; 
}
