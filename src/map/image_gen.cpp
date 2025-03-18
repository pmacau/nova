#include "../ext/stb_image/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../ext/stb_image/stb_image_write.h"

#include <cstring>
#include <unordered_map>
#include "map/image_gen.hpp"
#include "util/debug.hpp"

/*
------------
Image creation
-----------
*/

Image load_image(const std::string& file_path) {
    int w, h, c;
    unsigned char* data = stbi_load(file_path.c_str(), &w, &h, &c, 0);
    if (!data) {
        debug_printf(DebugType::GAME_INIT, "Could not load the tileset image\n");
    }
    return {w, h, c, data};
}

Image create_image(int w, int h, int channels) {
    unsigned char* data = new unsigned char[w * h * channels];
    return {w, h, channels, data};
}


/*
------------
Image manipulation helpers
-----------
*/

void copy_subimage(Image& src, Image& dst, Box& bounds) {
    for (int i = 0; i < bounds.h; i++) {
        int srcIdx = ((bounds.y + i) * src.w + bounds.x) * src.channels;
        int dstIdx = i * bounds.w * src.channels;
        std::memcpy(dst.data + dstIdx, src.data + srcIdx, bounds.w * src.channels);
    }
}

void paste_subimage(Image& src, Image& dst, Box& bounds) {
    for (int i = 0; i < src.h; i++) {
        int dstIdx = ((bounds.y + i) * dst.w + bounds.x) * dst.channels;
        int srcIdx = i * src.w * src.channels;
        std::memcpy(dst.data + dstIdx, src.data + srcIdx, src.w * src.channels);
    }
}

/*
------------
Background map creation
-----------
*/

std::unordered_map<std::string, std::pair<int, int>> tileset_map = {
    //--------------------------------------------------------------------
    {"GGGG", {0, 0}}, {"GGGW", {0, 1}}, {"GGWG", {0, 2}}, {"GWGG", {0, 3}},
    {"WGGG", {0, 4}}, {"SSGG", {0, 5}}, {"SGSG", {0, 6}}, {"WWGS", {0, 7}},
    //--------------------------------------------------------------------
    {"WWWW", {1, 0}}, {"WWWG", {1, 1}}, {"WWGW", {1, 2}}, {"WGWW", {1, 3}},
    {"GWWW", {1, 4}}, {"WWSS", {1, 5}}, {"SSWW", {1, 6}}, {"WWSG", {1, 7}},
    //--------------------------------------------------------------------
    {"SSSS", {2, 0}}, {"SSSW", {2, 1}}, {"SSWS", {2, 2}}, {"SWSS", {2, 3}},
    {"WSSS", {2, 4}}, {"WGWG", {2, 5}}, {"GWGW", {2, 6}}, {"SWGW", {2, 7}},
    //--------------------------------------------------------------------
    {"GGGS", {3, 0}}, {"GGSG", {3, 1}}, {"GSGG", {3, 2}}, {"SGGG", {3, 3}},
    {"GGSS", {3, 4}}, {"WSWS", {3, 5}}, {"SWSW", {3, 6}}, {"GWSW", {3, 7}},
    //--------------------------------------------------------------------
    {"WWWS", {4, 0}}, {"WWSW", {4, 1}}, {"WSWW", {4, 2}}, {"SWWW", {4, 3}},
    {"WWGG", {4, 4}}, {"GSGS", {4, 5}}, {"GGWW", {4, 6}}, {"WSWG", {4, 7}},
    //--------------------------------------------------------------------
    {"SSSG", {5, 0}}, {"SSGS", {5, 1}}, {"SGSS", {5, 2}}, {"GSSS", {5, 3}},
    {"WSSW", {5, 4}}, {"SWWS", {5, 5}}, {"GSSG", {5, 6}}, {"WGWS", {5, 7}},
    //--------------------------------------------------------------------
    {"SGGS", {6, 0}}, {"GSSW", {6, 1}}, {"SGWS", {6, 2}}, {"SWGS", {6, 3}},
    {"WSSG", {6, 4}}, {"SGWW", {6, 5}}, {"GSWW", {6, 6}}, {"RRRR", {6, 7}},
};
std::unordered_map<Terrain, std::string> byte_map = {
    {Terrain::WATER, "W"}, {Terrain::SAND, "S"}, {Terrain::GRASS, "G"}
};


void update_tile(
    GameMap& game_map, int row, int col,
    Box& tile_box
) {
    std::string tl_s, tr_s, bl_s, br_s;
    tl_s = byte_map[get_terrain(game_map[row][col])];
    tr_s = byte_map[get_terrain(game_map[row][col + 1])];
    bl_s = byte_map[get_terrain(game_map[row + 1][col])];
    br_s = byte_map[get_terrain(game_map[row + 1][col + 1])];

    std::string tile_str = tl_s + tr_s + bl_s + br_s;
    std::pair<int, int> coord =
        (tileset_map.find(tile_str) != tileset_map.end()) ? tileset_map[tile_str] : std::pair(6, 7);

    switch (get_biome(game_map[row][col])) {
        case B_FOREST:
            coord.first += (0 * 7);
            break;
        case B_ICE:
            coord.first += (1 * 7);
            break;
        case B_SAVANNA:
            coord.first += (2 * 7);
            break;
        case B_JUNGLE:
            coord.first += (3 * 7);
            break;
        case B_BEACH:
            coord.first += (4 * 7);
            break;
        default:
            break;
    }

    tile_box.x = coord.second  * tile_box.w;
    tile_box.y = coord.first * tile_box.h;
}

void create_background(GameMap& game_map) {
    int h = game_map.size() - 1;
    int w = game_map[0].size() - 1;

    Image src = load_image(textures_path("tile/tileset.png"));
    Image tile = create_image(TILE_SIZE, TILE_SIZE, src.channels);
    Image out = create_image(w * TILE_SIZE, h * TILE_SIZE, src.channels);
    std::memset(out.data, 255, out.w * out.h * out.channels);

    Box tile_box = {0, 0, tile.w, tile.h}; 
    Box out_box  = {0, 0, tile.w, tile.h};

    debug_printf(DebugType::WORLD_INIT, "Generating background map\n");
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            out_box.x = col * out_box.w;
            out_box.y = row * out_box.h;

            update_tile(game_map, row, col, tile_box);
            copy_subimage(src, tile, tile_box);
            paste_subimage(tile, out, out_box);
        }
    }

    int success = stbi_write_png(
        map_path("textured_map.png").c_str(),
        out.w, out.h, out.channels, out.data, out.w * out.channels
    );

    if (success) debug_printf(DebugType::GAME_INIT, "Wrote map file successfully.\n");
    else         debug_printf(DebugType::GAME_INIT, "Error writing map file.\n");

    stbi_image_free(src.data);
    delete[] tile.data;
    delete[] out.data;
}

void create_biome_map(GameMap& game_map) {
    int h = game_map.size() - 1;
    int w = game_map[0].size() - 1;

    Image out = create_image(w, h, 3);
    std::memset(out.data, 255, out.w * out.h * out.channels);

    debug_printf(DebugType::WORLD_INIT, "Generating biome map\n");
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            unsigned char r, g, b;
            switch (get_biome(game_map[row][col])) {
                case B_FOREST:
                    r = 62; g=137; b=72;
                    break;
                case B_BEACH:
                    r=184; g=188; b=78;
                    break;
                case B_SAVANNA:
                    r=120; g=138; b=50;
                    break;
                case B_ICE:
                    r=255; g=255; b=255;
                    break;
                case B_JUNGLE:
                    r=17; g=91; b=40;
                    break;
                default:
                    r=0; g=149; b=233;
            }
            int pixel_index = (row * w + col) * 3;
            out.data[pixel_index] = r;
            out.data[pixel_index + 1] = g;
            out.data[pixel_index + 2] = b;
        }
    }

    int success = stbi_write_png(
        map_path("biome_map.png").c_str(),
        out.w, out.h, out.channels, out.data, out.w * out.channels
    );

    if (success) debug_printf(DebugType::GAME_INIT, "Wrote biome map file successfully.\n");
    else         debug_printf(DebugType::GAME_INIT, "Error biome map file.\n");

    delete[] out.data;
}