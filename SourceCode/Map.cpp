#include "Map.h"
#include "map_items/Tile.h"
#include "map_items/Spike.h"
#include "map_items/Portal.h"
#include "Hero.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "data/DataCenter.h"

// 圖片路徑 
constexpr char TILE_IMAGE_PATH[] = "./assets/image/tile/red_block.png";
constexpr char SPIKE_IMAGE_PATH[] = "./assets/image/spike/triangle_gradient_no_margin.png";
constexpr char PORTAL_IMAGE_PATH[] = "./assets/image/portal/green_portal_no_gray_bg.png";



enum block_state{
    EMPTY, //0
    TILE, //1
    SPIKE, //2
    TRIGER, //3
    EMPTY_TILE, //4
    TILE_EMPTY, //5
    TILE_EMPTY_SPIKE,//6
    PORTAL, //7
    HERO, //8
    MOVE_SPIKE //9
};

// 構造函數
Map::Map() {}


// 初始化整個地圖，從 CSV 文件中讀取 2D 地圖數據
void Map::init() {
    DataCenter *DC = DataCenter::get_instance();
    //地圖路徑
    if (DC->is_in_start){
        constexpr char MAP_PATH_START[] = "./assets/map/game_start.csv";
        create_map(MAP_PATH_START);
        game_start = false;
    }
    else{
        destroy_map();
        constexpr char MAP_PATH[] = "./assets/map/tile_based_map_dynamic.csv";
        create_map(MAP_PATH);
    }   
}

// 繪製所有的方磚
void Map::draw() {
    DataCenter *DC = DataCenter::get_instance();
    for (auto &spike_ptr : DC->spikes) {
        Spike &spike = *spike_ptr;
        spike.draw();
    }
    for (auto &portal_ptr : DC->portals) {
        Portal &portal = *portal_ptr;
        portal.draw();
    }
        for (auto &tile_ptr : DC->tiles) {
        Tile &tile = *tile_ptr;
        tile.draw();
    }
}

// 更新整個地圖
void Map::update() {
    DataCenter *DC = DataCenter::get_instance();
    for (auto &tile_ptr : DC->tiles) {
        Tile &tile = *tile_ptr;
        tile.update();
    }
    for (auto &spike_ptr : DC->spikes) {
        Spike &spike = *spike_ptr;
        spike.update();
    }
    for (auto &portal_ptr : DC->portals) {
        Portal &portal = *portal_ptr;
        portal.update();
    }
    DC->remove_tile();   

}

void Map::create_map(const char* map_path) {
    	DataCenter *DC = DataCenter::get_instance();

    std::ifstream file(map_path); // 指定 CSV 文件的路徑
    if (!file.is_open()) {
        std::cerr << "Error: Could not open map CSV file." << std::endl;
    }

    
    const int window_width = DC->window_width; 
    const int window_height = DC->window_height;
    std::string line;
    std::vector<std::vector<int>> map_data;
    
    // 讀取 CSV 文件中的 2D 數據
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<int> row_data;
        
        while (std::getline(ss, cell, ',')) {
            row_data.push_back(std::stoi(cell)); // 將每個數據轉換為 int
        }
        map_data.push_back(row_data);
    }
    file.close();
    
    int rows = map_data.size();
    int cols = map_data[0].size();

    
    // 根據 n x m 切分整個遊戲視窗的區塊大小
    float tile_width = static_cast<float>(window_width) / cols;
    float tile_height = static_cast<float>(window_height) / rows;

    // 使用 2D 陣列中的數據來創建方磚

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            switch(map_data[i][j]){
                case block_state::EMPTY:
                    break;
                case block_state::TILE:
                    DC->tiles.emplace_back(create_tile(j * tile_width, i * tile_height, tile_width, tile_height, TILE_IMAGE_PATH, block_state::TILE));
                    break;
                case block_state::SPIKE:
                    DC->spikes.emplace_back(create_spike(j * tile_width, i * tile_height, tile_width, tile_height, SPIKE_IMAGE_PATH, block_state::SPIKE));
                    break;
                case block_state::TRIGER:
                    break;
                case block_state::EMPTY_TILE:
                    DC->tiles.emplace_back(create_tile(j * tile_width, i * tile_height, tile_width, tile_height, TILE_IMAGE_PATH, block_state::EMPTY_TILE));
                    break;
                case block_state::TILE_EMPTY:
                    DC->tiles.emplace_back(create_tile(j * tile_width, i * tile_height, tile_width, tile_height, TILE_IMAGE_PATH, block_state::TILE_EMPTY));
                    break;
                case block_state::TILE_EMPTY_SPIKE:
                    DC->tiles.emplace_back(create_tile(j * tile_width, i * tile_height, tile_width, tile_height, TILE_IMAGE_PATH, block_state::TILE_EMPTY_SPIKE));
                    DC->spikes.emplace_back(create_spike(j * tile_width, i * tile_height, tile_width, tile_height, SPIKE_IMAGE_PATH, block_state::TILE_EMPTY_SPIKE));
                    break;
                case block_state::PORTAL:
                    DC->portals.emplace_back(create_portal(j * tile_width, i * tile_height, tile_width, tile_height, PORTAL_IMAGE_PATH));
                    break;
                case block_state::HERO:
                    this->hero_x = j * tile_width;
                    this->hero_y = i * tile_height;
                    break;
                case block_state::MOVE_SPIKE:
                    DC->spikes.emplace_back(create_spike(j * tile_width, i * tile_height, tile_width, tile_height, SPIKE_IMAGE_PATH, block_state::MOVE_SPIKE));
                    break;
            }
        }
    }
}

void Map::destroy_map() {
    DataCenter *DC = DataCenter::get_instance();
    DC->reset();
}

Tile *Map::create_tile(float x, float y, float width, float height, const char* image_path, int type) {
    return new Tile(x, y, width, height, image_path, type);
}
Spike *Map::create_spike(float x, float y, float width, float height, const char* image_path, int type) {
    return new Spike(x, y, width, height, image_path, type);
}
Portal *Map::create_portal(float x, float y, float width, float height, const char* image_path) {
    return new Portal(x, y, width, height, image_path);
}

