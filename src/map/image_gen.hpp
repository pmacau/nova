#include "common.hpp"
#include "map/tile.hpp"

struct Image {
    int w, h, channels;
    unsigned char* data;
};

struct Box {
    int x, y, w, h;
};

Image load_image(std::string& file_path);
Image create_image(int w, int h, int channels);

void copy_subimage(Image& src, Image& dst, Box& imgBounds);
void paste_subimage(Image& src, Image& dst, Box& imgBounds);
void create_background(GameMap& game_map);
void create_biome_map(GameMap& game_map);