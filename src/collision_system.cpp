#include "collision_system.hpp"
#include "ui_system.hpp"
#include "music_system.hpp"
#include "util/debug.hpp"

CollisionSystem::CollisionSystem(entt::registry& reg, WorldSystem& world, PhysicsSystem& physics) :
	registry(reg),
	world(world), 
	physics(physics)
{
}



void CollisionSystem::step(float elapsed_ms)
{
  std::vector<entt::entity> destroy_entities;
	// TODO: likely refactor this if our collision system becomes more complicated which it will if we decide we want obstacles to not be considered entities 
	// between entities, also doesn't include projectiles yet. Also maybe implement a k-d tree to detect valid candidates.
	// All mobs

	//clear marks. 
	auto view = registry.view<MarkedCollision>();
	registry.remove<MarkedCollision>(view.begin(), view.end());

	auto mobs = registry.view<Mob>();
	// mob refresh
	for (auto mob : mobs) {
		auto& mob_ref = registry.get<Mob>(mob);
		if (mob_ref.hit_time > 0) {
			mob_ref.hit_time -= elapsed_ms / 1000;
		}
	}
	// views for all relevant components
	auto entities = registry.view<Motion, HitBox>(); 
	auto playerCheck = registry.view<Player>();
	auto obstacles = registry.view<Obstacle>();
	auto screens = registry.view<ScreenState>();

	for (auto entity : entities) { 
		// checks if ID is player
		if ((uint32_t)entt::entt_traits<entt::entity>::to_entity(playerCheck.front()) == (uint32_t)entt::entt_traits<entt::entity>::to_entity(entity)) {
			for (auto mob : mobs) {
				if (isContact(mob, entity, registry, 10)) {
					auto& player_ref = registry.get<Player>(entity);
					auto& mob_ref = registry.get<Mob>(mob);
					auto& screen = registry.get<ScreenState>(screens.front());
					if (mob_ref.hit_time <= 0) {
						player_ref.health -= MOB_DAMAGE;
						MusicSystem::playSoundEffect(SFX::HIT);


            UISystem::updatePlayerHealthBar(registry, player_ref.health);
						physics.knockback(entity, mob, 400);

						if (screens.size() > 0) {
							screen.darken_screen_factor = std::min(screen.darken_screen_factor + 0.33f, 1.0f);
						}

						if (player_ref.health <= 0) {
							screen.darken_screen_factor = 0;
							world.player_respawn();
						}


						mob_ref.hit_time = 1.f;
						
					}
				}
				// repelling force if overlap
				if (isContact(mob, entity, registry, 0)) {
					physics.suppress(entity, mob);
				}
			}
		}

		if (registry.all_of<Projectile>(entity)) {
			auto& projectile = registry.get<Projectile>(entity);
			for (auto mob_entity : mobs) {
				auto& mob = registry.get<Mob>(mob_entity);
				if (isContact(entity, mob_entity, registry, 0.f)) {
					destroy_entities.push_back(entity);
					mob.health -= projectile.damage;
					MusicSystem::playSoundEffect(SFX::HIT);

					UISystem::updateMobHealthBar(registry, mob_entity, true);
					if (mob.health <= 0) {
						for (auto healthbar_entity : registry.view<MobHealthBar>()) {
							auto& healthbar = registry.get<MobHealthBar>(healthbar_entity);
							if (healthbar.entity == mob_entity) {
								destroy_entities.push_back(healthbar_entity);
								break;
							}
						}
						UISystem::renderItem(registry, mob_entity);
						destroy_entities.push_back(mob_entity);
					}
					break;
				}
			}
		}

		// OBSTACLE SECTION OF COLLISION
		if (registry.all_of<Obstacle>(entity)) {
			continue; // shouldnt deal with obstacles and obstacles
		}
		
		for (auto obstacle : obstacles) {
			auto& o = registry.get<Obstacle>(obstacle); 
			if (registry.all_of<Ship>(obstacle) && registry.all_of<Projectile>(entity)) {
				continue;
			}
			if (!o.isPassable) handleBlock(entity, obstacle, registry);
		}
	}
	for (auto entity : destroy_entities) {
		debug_printf(DebugType::COLLISION, "Destroying entity (collision sys)\n");
		registry.destroy(entity);
	}
}
 



	// e1 is entity, e2 is obstacle
	void CollisionSystem::handleBlock(entt::entity e1, entt::entity e2, entt::registry& registry) {
		if (!isContact(e1, e2, registry, 0)) {
			return; //should do nothing
		}
		auto& m1 = registry.get<Motion>(e1);
		auto& h1 = registry.get<HitBox>(e1);
		auto& m2 = registry.get<Motion>(e2);
		auto& h2 = registry.get<HitBox>(e2);
		 //might want to refactor but for now here
		const float MAX_ALLOWED_PENETRATION = 0.f; // Threshold for penetration this is for clipping
		float overlapX = 0.f;
		float overlapY = 0.f;
		glm::vec2 mtv(0.f);
		if (h1.type == HitBoxType::HITBOX_RECT && h2.type == HitBoxType::HITBOX_RECT) {
			float halfWidth1 = h1.shape.rect.width / 2.0f;
			float halfHeight1 = h1.shape.rect.height / 2.0f;
			float halfWidth2 = h2.shape.rect.width / 2.0f;
			float halfHeight2 = h2.shape.rect.height / 2.0f;
			float cx1 = m1.position.x;
			float cy1 = m1.position.y;
			float cx2 = m2.position.x;
			float cy2 = m2.position.y;
			float deltaX = fabs(cx1 - cx2);
			float deltaY = fabs(cy1 - cy2);
			float combinedHalfWidth = halfWidth1 + halfWidth2;
			float combinedHalfHeight = halfHeight1 + halfHeight2;
			overlapX = combinedHalfWidth - deltaX;
			overlapY = combinedHalfHeight - deltaY;
			if (overlapX < overlapY) {
				mtv.x = (cx1 < cx2) ? -overlapX : overlapX;
			}
			else {
				mtv.y = (cy1 < cy2) ? -overlapY : overlapY;
			}
		}
		else if (h1.type == HitBoxType::HITBOX_CIRCLE && h2.type == HitBoxType::HITBOX_CIRCLE) {
			float r1 = h1.shape.circle.radius;
			float r2 = h2.shape.circle.radius;
			glm::vec2 diff = m1.position - m2.position;
			float distance = glm::length(diff);
			float penetration = (r1 + r2) - distance;
			overlapX = penetration;
			overlapY = penetration;
			if (distance > 0.f) {
				glm::vec2 normal = diff / distance;
				mtv = normal * penetration;
			}
			else {
				mtv = glm::vec2(1, 0) * penetration; 
			}

		} else if ((h1.type == HitBoxType::HITBOX_CIRCLE && h2.type == HitBoxType::HITBOX_RECT) ||
			(h1.type == HitBoxType::HITBOX_RECT && h2.type == HitBoxType::HITBOX_CIRCLE)) {
			const HitBox* circle;
			const Motion* circleMotion;
			const HitBox* rect;
			const Motion* rectMotion;
			if (h1.type == HitBoxType::HITBOX_CIRCLE) {
				circle = &h1;
				circleMotion = &m1;
				rect = &h2;
				rectMotion = &m2;
			}
			else {
				circle = &h2;
				circleMotion = &m2;
				rect = &h1;
				rectMotion = &m1;
			}
			float halfWidth = rect->shape.rect.width / 2.0f;
			float halfHeight = rect->shape.rect.height / 2.0f;
			float left = rectMotion->position.x - halfWidth;
			float right = rectMotion->position.x + halfWidth;
			float top = rectMotion->position.y - halfHeight;
			float bottom = rectMotion->position.y + halfHeight;
			float closestX = glm::clamp(circleMotion->position.x, left, right);
			float closestY = glm::clamp(circleMotion->position.y, top, bottom);
			glm::vec2 closestPoint = glm::vec2(closestX, closestY);
			glm::vec2 diff = circleMotion->position - closestPoint;
			float distance = glm::length(diff);
			float penetration = circle->shape.circle.radius - distance;
			overlapX = fabs(circleMotion->position.x - closestX);
			overlapY = fabs(circleMotion->position.y - closestY);

			if (distance > 0.f) {
				glm::vec2 normal = diff / distance;
				mtv = normal * penetration;

				// if h1 is the rectangle (e.g. the circle is h2), reverse the direction.
				if (h1.type == HitBoxType::HITBOX_RECT) {
					mtv = -mtv * 1.02f;
				}
			}
			else {
				// if centers are with rectangle edge, choose arbitrary direction.
				mtv = glm::vec2(1, 0) * (penetration + 0.05f);
			}
		} 
		
		m1.position = m1.position + mtv;
	
		vec2 normal = getNormal(m1, h1, m2, h2);
		float dot_product = dot(m1.velocity, normal);
		if (dot_product > 0) { //since if it's positive we are facing away
			return;
		}
		vec2 velocity_Component = dot_product * normal * 1.5f;
		if (!registry.all_of<MarkedCollision>(e1)) {
			auto& mark = registry.emplace<MarkedCollision>(e1);
			mark.velocity = m1.velocity - velocity_Component;
		}
		
	}



	

	// gets the normal of either circle rect, or rect rect cannot handle complex shapes (plan to refactor later)
	vec2 CollisionSystem::getNormal(const Motion& m1, const HitBox& h1, const Motion& m2, const HitBox& h2) {
		if (h1.type == HitBoxType::HITBOX_CIRCLE && h2.type == HitBoxType::HITBOX_CIRCLE) {
			return normalize(m1.position - m2.position); 
		}
		// both rectangles
		if (h1.type == HitBoxType::HITBOX_RECT && h2.type == HitBoxType::HITBOX_RECT) {
				float halfWidth1 = h1.shape.rect.width / 2.0f;
				float halfHeight1 = h1.shape.rect.height / 2.0f;
				float halfWidth2 = h2.shape.rect.width / 2.0f;
				float halfHeight2 = h2.shape.rect.height / 2.0f;
				float cx1 = m1.position.x;
				float cy1 = m1.position.y;
				float cx2 = m2.position.x;
				float cy2 = m2.position.y;
				float deltaX = cx1 - cx2;
				float deltaY = cy1 - cy2;
				float combinedHalfWidth = halfWidth1 + halfWidth2;
				float combinedHalfHeight = halfHeight1 + halfHeight2;
				float overlapX = combinedHalfWidth - fabs(deltaX);
				float overlapY = combinedHalfHeight - fabs(deltaY);
				if (overlapX < 0 || overlapY < 0)
					return glm::vec2(0, 0);
				if (overlapX < overlapY) {
					return (deltaX < 0) ? glm::vec2(-1, 0) : glm::vec2(1, 0);
				}
				else {
					return (deltaY < 0) ? glm::vec2(0, -1) : glm::vec2(0, 1);
				}
		}
		// rectangle circle collision 
		if ((h1.type == HitBoxType::HITBOX_RECT || h1.type == HitBoxType::HITBOX_CIRCLE) && (h2.type == HitBoxType::HITBOX_RECT || h2.type == HitBoxType::HITBOX_CIRCLE)) {
			if (h1.type == HitBoxType::HITBOX_RECT) {
				return normalize(rectangleClamp(m1, h1, m2, h2) - m1.position);
			}
			else {
				return normalize(rectangleClamp(m2, h2, m1, h1) - m2.position);
			}
		}

		// should never reach...
		return {0,0};
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
				return true; 
			}
		}
		else {
			closestPoint = rectangleClamp(m1, h1, m2, h2);
			if (glm::length(closestPoint - m2.position) < h2.shape.circle.radius) {
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



