#include "map_system.hpp"
#include "world_init.hpp"

/*
--------------------
Helpers
--------------------
*/

void createBackground(entt::registry& reg, int width, int height, int tile_size) {
    auto background_ents = reg.view<Background>();
    reg.destroy(background_ents.begin(), background_ents.end());

    auto entity = reg.create();
    reg.emplace<Background>(entity);
    
    auto& sprite = reg.emplace<Sprite>(entity);
    sprite.coord = {0.0f, 0.0f};
    sprite.dims = {tile_size * (width - 1), tile_size * (height - 1)};
    sprite.sheet_dims = {tile_size * (width - 1), tile_size * (height - 1)};

    auto& motion = reg.emplace<Motion>(entity);
    motion.position = 0.5f * sprite.dims; // shifts top-left corner to (0, 0)
    motion.scale = {tile_size * (width - 1), tile_size * (height - 1)};

    auto& renderRequest = reg.emplace<RenderRequest>(entity);
    renderRequest.used_effect = EFFECT_ASSET_ID::TEXTURED;
    renderRequest.used_geometry = GEOMETRY_BUFFER_ID::SPRITE;
    renderRequest.used_texture = TEXTURE_ASSET_ID::MAP_BACKGROUND;
};

/*
--------------------
Public methods
--------------------
*/

void MapSystem::init(entt::registry& reg) {
    loadMap();
    createBackground(reg, MAP_WIDTH, MAP_HEIGHT, TILE_SIZE);
};

void MapSystem::generate_new_map() {
    // TODO: port python map generator to cpp
}

vec2 MapSystem::populate_ecs(entt::registry& reg) {
    vec2 spawn_pos = {0.f, 0.f};

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            Tile tile = static_cast<Tile>(game_map[i][j]);
            vec2 map_pos = float(TILE_SIZE) * vec2(j, i);

            switch (tile) {
                case Tile::BOSS_SPAWN:
                    createBoss(reg, map_pos);
                    break;
                case Tile::SPAWN:
                    spawn_pos = map_pos;
                    break;
                case Tile::TREE:
                    createTree(reg, map_pos);
                    break;
                default:
                    break;
            }
        }
    }
    return spawn_pos;
};

void MapSystem::update_location(entt::registry& reg, entt::entity ent) {
    if (!reg.all_of<Motion>(ent)) return;

    auto& motion = reg.get<Motion>(ent);
    vec2& pos = motion.position;
    vec2& formerPos = motion.formerPosition;

    Tile curr = get_tile(vec2(pos.x, pos.y)       + motion.offset_to_ground);
    Tile newX = get_tile(vec2(pos.x, formerPos.y) + motion.offset_to_ground);
    Tile newY = get_tile(vec2(formerPos.x, pos.y) + motion.offset_to_ground);

    if (!walkable_tile(curr)) {
        if (walkable_tile(newX)) {
            motion.position = {pos.x, formerPos.y};
        }
        else if (walkable_tile(newY)) {
            motion.position = {formerPos.x, pos.y};
        }
        else {
            motion.position = formerPos;
        }
    }
};

/*
--------------------
Private methods
--------------------
*/

void MapSystem::loadMap() {
    game_map.assign(MAP_HEIGHT, std::vector<uint8_t>(MAP_WIDTH));

    std::ifstream file(map_path("map.bin"), std::ios::binary);
    if (file) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            file.read(reinterpret_cast<char*>(game_map[y].data()), MAP_WIDTH);
        }
    } else {
        debug_printf(DebugType::WORLD_INIT, "Could not find map file!\n");
    }
};

Tile MapSystem::get_tile(vec2 pos) {
    int tile_x = std::round(pos.x / TILE_SIZE);
    int tile_y = std::round(pos.y / TILE_SIZE);
    if (tile_x >= 0 && tile_x < MAP_WIDTH && tile_y >= 0 && tile_y < MAP_HEIGHT) {
        return static_cast<Tile>(game_map[tile_y][tile_x]);
    }
    return Tile::TILE_COUNT;
};

bool MapSystem::walkable_tile(Tile tile) {
    return (
        tile != Tile::WATER && tile != Tile::TREE
    );
};