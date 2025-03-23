#include "map_system.hpp"
#include "world_init.hpp"
#include "music_system.hpp"

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

    auto& color = reg.emplace<vec3>(entity);
    color = {1.f, 1.f, 1.f};
    
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
    createBackground(reg, map_width, map_height, TILE_SIZE);
};

void MapSystem::generate_new_map() {
    // TODO: port python map generator to cpp
}

vec2 MapSystem::populate_ecs(
    entt::registry& reg,
    vec2& p_pos,
    vec2& s_pos
) {
    vec2 spawn_pos = {0, 0};

    for (int i = 0; i < map_height; i++) {
        for (int j = 0; j < map_width; j++) {
            vec2 map_pos = float(TILE_SIZE) * vec2(j, i);

            switch (get_decoration(game_map[i][j])) {
                case Decoration::BOSS:
                    bossSpawnIndices.push_back(vec2(j, i));
                    break;
                case Decoration::SPAWN:
                    p_pos = map_pos;
                    break;
                case Decoration::TREE:
                    if (get_terrain(game_map[i][j]) == Terrain::SAND) {
                        createTree(reg, map_pos, {0, 1});
                    } else {
                        switch (get_biome(game_map[i][j])) {
                            case B_FOREST:
                                createTree(reg, map_pos, {0, 0});
                                break;
                            case B_SAVANNA:
                                createTree(reg, map_pos, {0, 4});
                                break;
                            case B_ICE:
                                createTree(reg, map_pos, {0, 3});
                                break;
                            case B_JUNGLE:
                                createTree(reg, map_pos, {0, 5});
                                break;
                            default:
                                createTree(reg, map_pos, {0, 2});
                                break;
                        }
                    }
                    break;
                case Decoration::SHIP:
                    s_pos = map_pos;
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

void MapSystem::update_background_music(entt::registry& reg, entt::entity ent) {
    if (!reg.all_of<Motion>(ent)) return;
    //auto& screen = reg.get<ScreenState>(reg.view<ScreenState>().front());

    auto& motion = reg.get<Motion>(ent);
    vec2& pos = motion.position;
    vec2& formerPos = motion.formerPosition;

    Tile currT = get_tile(pos + motion.offset_to_ground);
    Tile prevT = get_tile(formerPos + motion.offset_to_ground);

    Biome currB = get_biome(currT);
    Biome prevB = get_biome(prevT);

    if (currB == prevB || currB == B_OCEAN) return;
    Music newTrack;

    switch (currB) {
        case B_FOREST:
            newTrack = Music::FOREST;
            break;  
        case B_BEACH:
            newTrack = Music::BEACH;
            break;
        case B_JUNGLE:
            newTrack = Music::JUNGLE;
            break;
        case B_SAVANNA:
            newTrack = Music::SAVANNA;
            break;
        case B_ICE:
            newTrack = Music::SNOWLANDS;
            break;
        default:
            newTrack = Music::FOREST;
            break;
    }

    MusicSystem::playMusic(newTrack);
}

/*
--------------------
Private methods
--------------------
*/

void MapSystem::loadMap() {
    std::ifstream file(map_path("map.bin"), std::ios::binary);
    if (!file) {
        debug_printf(DebugType::WORLD_INIT, "Could not find map file!\n");
        return;
    }

    // Read dimensions
    uint32_t rows, cols;
    file.read(reinterpret_cast<char*>(&rows), sizeof(rows));
    file.read(reinterpret_cast<char*>(&cols), sizeof(cols));

    map_height = rows;
    map_width  = cols;

    // Read data
    game_map.assign(rows, std::vector<Tile>(cols));
    for (auto& row: game_map) {
        file.read(reinterpret_cast<char*>(row.data()), cols * sizeof(Tile));
    }
    file.close();
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
    if (tile_x >= 0 && tile_x < map_width && tile_y >= 0 && tile_y < map_height) {
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

Biome MapSystem::get_biome_by_indices(ivec2 tile_indices) {
    return get_biome(get_tile_type_by_indices(tile_indices.x, tile_indices.y));
};


std::vector<vec2>& MapSystem::getBossSpawnIndices() {
    return bossSpawnIndices;
}