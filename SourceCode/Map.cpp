#include "Map.h"
#include "Tile.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "data/DataCenter.h"

// Tile 圖片路徑 (可修改為您想要的圖片路徑)
constexpr char TILE_IMAGE_PATH[] = "./assets/image/tile/red_block.png";

// 構造函數
Map::Map() {}



// 初始化整個地圖，從 CSV 文件中讀取 2D 地圖數據
void Map::init() {
	DataCenter *DC = DataCenter::get_instance();

    std::ifstream file("./assets/map/tile_based_map_dynamic.csv"); // 指定 CSV 文件的路徑
    if (!file.is_open()) {
        std::cerr << "Error: Could not open map CSV file." << std::endl;
    }

    
    const int window_width = DC->window_width; 
    const int window_height = DC->window_width;
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
            if (map_data[i][j] == 1) { // 1 代表有方磚，0 代表沒有方磚
                float x = j * tile_width;
                float y = i * tile_height;
                DC->tiles.emplace_back(create_tile(x, y, tile_width, tile_height, TILE_IMAGE_PATH));
            }
        }
    }
}

// 繪製所有的方磚
void Map::draw() {
    DataCenter *DC = DataCenter::get_instance();
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
}

Tile *Map::create_tile(float x, float y, float width, float height, const char* image_path) {
    return new Tile(x, y, width, height, image_path);
}

