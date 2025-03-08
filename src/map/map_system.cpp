#include "map_system.hpp"
#include "world_init.hpp"

/*
--------------------
Helpers
--------------------
*/

std::vector<vec2> MapSystem::bossSpawnIndices;

void createBackground(entt::registry& reg, int width, int height, int tile_size) {
    auto background_ents = reg.view<Background>();
    reg.destroy(background_ents.begin(), background_ents.end());

    auto entity = reg.create();
    reg.emplace<Background>(entity);
    
    auto& sprite = reg.emplace<Sprite>(entity);
    sprite.coord = {0, 0};
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
    vec2 spawn_pos = {0, 0};

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            vec2 map_pos = float(TILE_SIZE) * vec2(j, i);

            switch (get_decoration(game_map[i][j])) {
                case Decoration::BOSS:
                    bossSpawnIndices.push_back(vec2(j, i));
                    break;
                case Decoration::SPAWN:
                    spawn_pos = map_pos;
                    break;
                case Decoration::TREE:
                    if (get_terrain(game_map[i][j]) == Terrain::SAND) {
                        createTree(reg, map_pos, {0, 1});
                    } else {
                        createTree(reg, map_pos, {0, 0});
                    }
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
    vec2 tile_indices = get_tile_indices(pos);
    int tile_x = tile_indices.x;
    int tile_y = tile_indices.y;
    return MapSystem::get_tile_type_by_indices(tile_x, tile_y);
};


vec2 MapSystem::get_tile_indices(vec2 pos) {
    return vec2(std::round(pos.x / TILE_SIZE), std::round(pos.y / TILE_SIZE));
};

Tile MapSystem::get_tile_type_by_indices(int tile_x, int tile_y) {
    if (tile_x >= 0 && tile_x < MAP_WIDTH && tile_y >= 0 && tile_y < MAP_HEIGHT) {
        return game_map[tile_y][tile_x];
    }
    Tile t;
    set_terrain(t, Terrain::WATER);
    return t;
};

vec2 MapSystem::get_tile_center_pos(vec2 tile_indices) {
    float tile_size = static_cast<float>(TILE_SIZE);
    return tile_size * (tile_indices + vec2(0.5f));
}

bool MapSystem::walkable_tile(Tile tile) {
    return (
        get_terrain(tile) != Terrain::WATER &&
        get_decoration(tile) != Decoration::TREE 
    );
};


std::vector<vec2>& MapSystem::getBossSpawnIndices() {
    return bossSpawnIndices;
}

// void MapSystem::removeBossSpawnIndex(const vec2& tileIndices) {
//     auto it = std::find(bossSpawnIndices.begin(), bossSpawnIndices.end(), tileIndices);
//     if (it != bossSpawnIndices.end())
//         bossSpawnIndices.erase(it);
// }