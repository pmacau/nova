#include "generate.hpp"

/*
--------------------
Terrain generation
--------------------
*/

FastNoiseLite setup_noise(NoiseParams params) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(1.0 / params.scale);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(params.octaves);
    noise.SetFractalGain(params.persistence);
    noise.SetFractalLacunarity(params.lacunarity);

    return noise;
}

Tile discretize(double val) {
    Tile t = 0;

    double bins[] = {0, 0.33, 0.4, 1};
    if      (val < bins[1]) set_terrain(t, Terrain::WATER);
    else if (val < bins[2]) set_terrain(t, Terrain::SAND);
    else                    set_terrain(t, Terrain::GRASS);

    return t;
}

GameMap generate_terrain(
    int width, int height, NoiseParams params
) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-10000, 10000);
    double offset_x = dist(gen);
    double offset_y = dist(gen);

    std::vector<std::vector<uint8_t>> terrain(height, std::vector<uint8_t>(width, 0.0));
    FastNoiseLite noise = setup_noise(params);

    int cx = width / 2, cy = height / 2;
    double max_dist = sqrt(cx * cx + cy * cy);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double val = noise.GetNoise(float(j + offset_x), float(i + offset_y));
            double dist = sqrt((i - cx) * (i - cx) + (j - cy) * (j - cy)) / max_dist;

            val  = 0.5 * (val + 1.0);
            dist = std::max(0.0, 1.0 - dist * dist);

            terrain[i][j] = discretize(val * dist);
        }
    }

    GameMap padded(height, std::vector<Tile>(width, (Tile)0));
    for (int i = 0; i < height - 2; ++i) {
        for (int j = 0; j < width - 2; ++j) {
            padded[i + 1][j + 1] = terrain[i][j];
        }
    }
    
    return padded;
}

/*
-----------------------
Map decoration helpers
-----------------------
*/

std::pair<int, int> find_valid_area(
    const GameMap& terrain,
    const std::pair<int, int>& start,
    int range
) {
    int height = terrain.size(), width = terrain[0].size();

    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    std::queue<std::pair<int, int>> queue;

    queue.push(start);
    visited[start.first][start.second] = true;

    auto process = [&queue, &visited, width, height](int row, int col) {
        if (
            (0 <= row && row < height) &&
            (0 <= col && col < width) &&
            !visited[row][col]
        ) {
            visited[row][col] = true;
            queue.push({row, col});
        }
    };

    auto is_valid = [&terrain, width, height](std::pair<int, int> curr, int range) {
        int min_row = max(0, curr.first - range / 2);
        int max_row = min(height, curr.first + range / 2);

        int min_col = max(0, curr.second - range / 2);
        int max_col = min(width, curr.second + range / 2);

        for (int i = min_row; i < max_row; i++) {
            for (int j = min_col; j < max_col; j++) {
                if (
                    get_terrain(terrain[i][j]) == Terrain::WATER ||
                    get_decoration(terrain[i][j]) == Decoration::SPAWN
                ) {
                    return false;
                }
            }
        }
        return true;
    };

    while (!queue.empty()) {
        auto curr = queue.front();
        queue.pop();

        if (is_valid(curr, range)) return curr;

        int row = curr.first, col = curr.second;
        process(row - 1, col - 1);
        process(row - 1, col + 1);
        process(row + 1, col - 1);
        process(row + 1, col + 1);
    }

    debug_printf(DebugType::GAME_INIT, "Bad news, aborting\n");
    return start;
}

std::pair<int, int> player_spawn(
    const GameMap& terrain, int width, int height
) {
    int c_row = height / 2, c_col = width / 2;
    return find_valid_area(terrain, {c_row, c_col}, 5);
}

std::pair<int, int> ship_spawn(
    const GameMap& terrain, const std::pair<int, int>& player_spawn
) {
    return find_valid_area(
        terrain,
        {player_spawn.first - 15, player_spawn.second},
        10
    );
}


std::vector<std::vector<bool>> find_mainland(
    const GameMap& terrain, std::pair<int, int> player_spawn
) {
    int height = terrain.size(), width = terrain[0].size();
    int row = player_spawn.first, col = player_spawn.second;

    std::vector<std::vector<bool>> mainland(height, std::vector<bool>(width, false));
    std::queue<std::pair<int, int>> queue;

    queue.push({row, col});
    mainland[row][col] = true;

    auto process = [&queue, &mainland, &terrain, width, height](int row, int col) {
        if (
            (0 <= row && row < height) &&
            (0 <= col && col < width) &&
            !mainland[row][col] &&
            get_terrain(terrain[row][col]) != Terrain::WATER
        ) {
            mainland[row][col] = true;
            queue.push({row, col});
        }
    };

    while (!queue.empty()) {
        auto curr = queue.front();
        queue.pop();

        int row = curr.first, col = curr.second;
        
        process(row - 1, col - 1);
        process(row - 1, col + 1);
        process(row + 1, col - 1);
        process(row + 1, col + 1);
    }

    return mainland;
}

int manhattan_distance(int r1, int c1, int r2, int c2) {
    return std::abs(r1 - r2) + std::abs(c1 - c2);
}

std::vector<std::pair<int, int>> find_biome_seeds(
    const std::vector<std::vector<bool>>& mainland, std::pair<int, int> spawn, int k
) {
    int height = mainland.size(), width = mainland[0].size();
    
    std::vector<std::pair<int, int>> selected_points;
    selected_points.push_back({spawn.first, spawn.second});
    
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    visited[spawn.first][spawn.second] = true;
    
    auto get_min_distance_to_selected = [&](int r, int c) {
        int min_dist = std::numeric_limits<int>::max();
        for (const auto& p : selected_points) {
            int dist = manhattan_distance(r, c, p.first, p.second);
            min_dist = std::min(min_dist, dist);
        }
        return min_dist;
    };
    
    for (int i = 0; i < k; i++) {
        int max_distance = -1;
        std::pair<int, int> next_point;
        
        for (int r = 0; r < height; r++) {
            for (int c = 0; c < width; c++) {
                if (!mainland[r][c] || visited[r][c]) continue;
            
                int dist = get_min_distance_to_selected(r, c);
                
                if (dist > max_distance) {
                    max_distance = dist;
                    next_point = {r, c};
                }
            }
        }
        
        visited[next_point.first][next_point.second] = true;
        selected_points.push_back(next_point);
    }
    
    return selected_points;
}

std::vector<std::pair<int, int>> get_trees(const GameMap& terrain, int num_trees, int min_dist) {
    int height = terrain.size(), width = terrain[0].size();
    
    std::vector<std::pair<int, int>> land_positions;
    std::vector<std::pair<int, int>> trees;

    auto is_valid = [&terrain, width, height](int r, int c) {
        if (get_terrain(terrain[r][c]) == Terrain::WATER) return false;

        int range = 10;
        int min_row = max(0, r - range), max_row = min(height, r + range);
        int min_col = max(0, c - range), max_col = min(width, c + range);

        for (int i = min_row; i < max_row; i++) {
            for (int j = min_col; j < max_col; j++) {
                if (get_decoration(terrain[i][j]) != Decoration::NO_DECOR) {
                    return false;
                }
            }
        }
        return true;
    };

    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            if (is_valid(r, c)) {
                land_positions.emplace_back(r, c);
            }
        }
    }

    auto valid_pos = [&trees, min_dist](int p_row, int p_col) {
        for (const auto& tree : trees) {
            if (manhattan_distance(p_row, p_col, tree.first, tree.second) < min_dist) {
                return false;
            }
        }
        return true;
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(land_positions.begin(), land_positions.end(), gen);

    for (const auto& pos : land_positions) {
        if (trees.size() >= num_trees) break;

        if (valid_pos(pos.first, pos.second)) {
            trees.push_back(pos);
        }
    }

    return trees;
}

/*
-----------------------
Full map helpers
-----------------------
*/

GameMap create_map(int width, int height) {
    auto terrain = generate_terrain(width, height, {});
    debug_printf(DebugType::WORLD_INIT, "Created terrain map\n");

    auto spawn = player_spawn(terrain, width, height);
    set_decoration(terrain[spawn.first][spawn.second], Decoration::SPAWN);
    debug_printf(DebugType::WORLD_INIT, "Setting player spawn at: (%d, %d)\n", spawn.first, spawn.second);

    auto ship = ship_spawn(terrain, spawn);
    set_decoration(terrain[ship.first][ship.second], Decoration::SHIP);
    debug_printf(DebugType::WORLD_INIT, "Setting ship spawn at: (%d, %d)\n", ship.first, ship.second);

    auto mainland = find_mainland(terrain, spawn);
    auto biome_seeds = find_biome_seeds(mainland, spawn, 4);
    for (int i = 1; i <= 4; i++) {
        auto seed = biome_seeds[i];
        set_decoration(terrain[seed.first][seed.second], Decoration::BOSS);
    }
    debug_printf(DebugType::WORLD_INIT, "Planted biome seeds\n");

    auto trees = get_trees(terrain, 1500, 15);
    for (const auto& tree : trees) {
        set_decoration(terrain[tree.first][tree.second], Decoration::TREE);
    }
    debug_printf(DebugType::WORLD_INIT, "Planted %d trees\n", trees.size());

    return terrain;
}


void save_map(const GameMap& map, const char* filepath) {
    std::ofstream file(filepath, std::ios::binary);

    // Store map dimensions first
    uint32_t rows = map.size();
    uint32_t cols = rows > 0 ? map[0].size() : 0;
    file.write(reinterpret_cast<const char*>(&rows), sizeof(rows));
    file.write(reinterpret_cast<const char*>(&cols), sizeof(cols));

    for (const auto& row : map) {
        file.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(Tile));
    }
    file.close();
}