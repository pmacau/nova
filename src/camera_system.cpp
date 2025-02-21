
#include "camera_system.hpp"

CameraSystem::CameraSystem(entt::registry& reg, WorldSystem& world) :
	registry(reg),
	world(world)
{
}

mat3 rotation_matrix_X(float angle) {
    return mat3{ {1, 0, 0}, {0, cos(angle), -sin(angle)}, {0, sin(angle), cos(angle)} };
}

mat3 rotation_matrix_Y(float angle) {
    return mat3{ {cos(angle), 0, sin(angle)}, {0, 1, 0}, {-sin(angle), 0, cos(angle)} };
}

mat3 rotation_matrix_Z(float angle) {
    return mat3{ {cos(angle), -sin(angle), 0}, {sin(angle), cos(angle), 0}, {0, 0, 1} };
}

vec3 rotateX(vec3 v, float angle, vec3 origin) {
    return (rotation_matrix_X(angle) * (v - origin)) + origin;
}

void CameraSystem::step(float elapsed_ms) {
    auto camera_entity = registry.view<Camera>().front(); // TODO: make this more robust
    auto& camera = registry.get<Camera>(camera_entity);

    assert(registry.any_of<Motion>(camera.target));
    auto& target_motion = registry.get<Motion>(camera.target);

    // Camera follows player
    camera.offset = target_motion.position;

    // update camera position: it is camera.distance_from_target away from the target and tilted by camera.angle
    camera.position = vec3(target_motion.position, camera.distance_from_target);

    // rotate camera around target
    camera.position = rotateX(camera.position, camera.angle, vec3(target_motion.position, 0));

    for (auto entity : registry.view<UI>()) {
        auto& ui_motion = registry.get<Motion>(entity);
        if (registry.any_of<PlayerHealthBar>(entity)) {
            ui_motion.position = { target_motion.position.x + WINDOW_WIDTH_PX / 2 - 125.f - 50.f,
                                  target_motion.position.y - WINDOW_HEIGHT_PX / 2 + 50.f };
        }
    }
}