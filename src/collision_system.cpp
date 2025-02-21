#include "collision_system.hpp"


CollisionSystem::CollisionSystem(entt::registry& reg, WorldSystem& world, PhysicsSystem& physics) :
	registry(reg),
	world(world), 
	physics(physics)
{
}



void CollisionSystem::step(float elapsed_ms)
{
	// TODO: likely refactor this if our collision system becomes more complicated which it will if we decide we want obstacles to not be considered entities 
	// between entities, also doesn't include projectiles yet. Also maybe implement a k-d tree to detect valid candidates.
	float elapsed_s = elapsed_ms / 1000.f;
	// All mobs
	auto mobs = registry.view<Mob>();
	// mob refresh
	for (auto mob : mobs) {
		auto& mob_ref = registry.get<Mob>(mob);
		if (mob_ref.hit_time > 0) {
			mob_ref.hit_time -= elapsed_s;
		}
	}
	auto entities = registry.view<Motion, HitBox>(); // only goes through motion objects with HitBox (should essentially be all of them)
	auto playerCheck = registry.view<Player>();
	for (auto entity : entities) { // does a for loop for projectiles and what not right, now this isn't necessary. 
		// only check once so decide if checks on player or invader (player chosen)
		// checks if ID is player
		if ((uint32_t)entt::entt_traits<entt::entity>::to_entity(playerCheck.front()) == (uint32_t)entt::entt_traits<entt::entity>::to_entity(entity)) {
			for (auto mob : mobs) {
				//std::cout << "ENTERED" << std::endl;
				
				if (isContact(mob, entity, registry, 10)) {
					auto& player_ref = registry.get<Player>(entity);
					auto& mob_ref = registry.get<Mob>(mob);
					if (mob_ref.hit_time <= 0) {
						std::cout << "COLLISION" << std::endl;
						player_ref.health -= MOB_DAMAGE; // FOR DEBUGGING
						physics.knockback(entity, mob, 400);
						if (player_ref.health <= 0) {
							world.player_respawn();
						}
						mob_ref.hit_time = 1.f;
						
					}
				}
				// repelling force if overlap
				if (isContact(mob, entity, registry, 0)) {
					std::cout << "SUPRESS" << std::endl;
					physics.suppress(entity, mob);
				}
				
				
				


			}
		}
		auto obstacles = registry.view<Obstacle>(); 
		//might want to refactor for performance but for now it's here. 
		if (registry.all_of<Obstacle>(entity)) {
			continue; // shouldnt deal with obstacles and obstacles
		}
		for (auto obstacle : obstacles) {
			handleBlock(entity, obstacle); 
		}


		

	}
}

	void CollisionSystem::handleblock(entt::entity e1, entt::entity e2) {
		auto& motion = registry.get<Motion>(entity);
		getNormal()
		}

	// gets the normal of either circle rect, or rect rect cannot handle complex shapes (plan to refactor later)
	bool CollisionSystem::getNormal(const Motion& m1, const HitBox& h1, const Motion& m2, const HitBox& h2) {

	}


	// refactor this so it uses the hitboxes.
	// ASSUMES ENTIT
	// determines if should get hit or not maybe just refactor into hit box system after.
	bool CollisionSystem::isContact(entt::entity e1, entt::entity e2, entt::registry & registry, float epsilon) {
		const Motion& m1 = registry.get<Motion>(e1);
		const Motion& m2 = registry.get<Motion>(e2);
		const HitBox& h1 = registry.get<HitBox>(e1); // can be a circle or a rectangle
		const HitBox& h2 = registry.get<HitBox>(e2); 
		// can potentially due a epsilon check here 
		// cases two circles
		if (h1.type == HitBoxType::HITBOX_CIRCLE && h2.type == HitBoxType::HITBOX_CIRCLE) {
			return circlesCollision(m1, h1, m2, h2, epsilon);
		}
		// case two rectangles
		if (h1.type == HitBoxType::HITBOX_RECT && h2.type == HitBoxType::HITBOX_RECT) {
			return RectRectCollision(m1, h1, m2, h2, epsilon);
		}
		// case one circle and two rectangles 
		if ((h1.type == HitBoxType::HITBOX_RECT || h1.type == HitBoxType::HITBOX_CIRCLE) && (h2.type == HitBoxType::HITBOX_RECT || h2.type == HitBoxType::HITBOX_CIRCLE)) {
			return circleRectCollision(m1, h1, m2, h2, epsilon);
		}

		return false; 
	}

	bool CollisionSystem::circlesCollision(const Motion& m1, const HitBox& h1, const Motion& m2, const HitBox& h2, float epsilon) {
		float dx = m1.position.x - m2.position.x;
		float dy = m1.position.y - m2.position.y;
		float rSum = h1.shape.circle.radius + h2.shape.circle.radius + epsilon;
		return (dx * dx + dy * dy) <= (rSum * rSum); //avoiding sqrt 
	}

	// TODO
	bool CollisionSystem::circleRectCollision(const Motion& m1, const HitBox& h1, const Motion& m2, const HitBox& h2, float epsilon) {
		bool h1IsCircle = false; // if true h1 is a circle, if false h1 is a rectangle, and h2 is circle. 
		if (h1.type == HitBoxType::HITBOX_CIRCLE) {
			h1IsCircle = true;
		}
		vec2 closestPoint; 
		if (h1IsCircle) {
			closestPoint = rectangleClamp(m2, h2, m1, h1);
			if (glm::length(closestPoint - m1.position) < h1.shape.circle.radius) {
				std::cout << "Should det" << std::endl;
				return true; 
			}
		}
		else {
			closestPoint = rectangleClamp(m1, h1, m2, h2);
			if (glm::length(closestPoint - m2.position) < h2.shape.circle.radius) {
				std::cout << "Should det" << std::endl;
				return true;
			}
		}
		return false; 
	}



	bool CollisionSystem::RectRectCollision(const Motion& m1, const HitBox& h1, const Motion& m2, const HitBox& h2, float epsilon) {
		float left1 = m1.position.x - h1.shape.rect.width / 2;
		float right1 = m1.position.x + h1.shape.rect.width / 2;
		float top1 = m1.position.y - h1.shape.rect.height / 2;
		float bottom1 = m1.position.y + h1.shape.rect.height / 2;
		float left2 = m2.position.x - h2.shape.rect.width / 2;
		float right2 = m2.position.x + h2.shape.rect.width / 2;
		float top2 = m2.position.y - h2.shape.rect.height / 2;
		float bottom2 = m2.position.y + h2.shape.rect.height / 2;
		bool overlapX = (left1 < right2 + epsilon) && (right1 + epsilon > left2);
		bool overlapY = (top1 < bottom2 + epsilon) && (bottom1 + epsilon > top2);
		return overlapX && overlapY;
	}

	vec2 CollisionSystem::rectangleClamp(const Motion& rect_motion, const HitBox& rect_hitbox, const Motion& circle_motion, const HitBox& circle_hitbox) {
		float maxLeft = rect_motion.position.x - rect_hitbox.shape.rect.width / 2;
		float maxRight = rect_motion.position.x + rect_hitbox.shape.rect.width / 2;
		float maxTop = rect_motion.position.y - rect_hitbox.shape.rect.height / 2;
		float maxBottom = rect_motion.position.y + rect_hitbox.shape.rect.height / 2;
		float closestXPoint = glm::clamp(circle_motion.position.x, maxLeft, maxRight);
		float closestYPoint = glm::clamp(circle_motion.position.y, maxTop, maxBottom);
		return vec2(closestXPoint, closestYPoint);
	}

	// PLUS ADD OBSTACLE TYPE COLLISION IN PHYSICS 

