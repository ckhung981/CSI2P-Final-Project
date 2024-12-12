#include "Tile.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "shapes/Rectangle.h"
#include <iostream>
#include <cstdio>

// 構造函數 (初始化位置、大小和載入圖片)
Tile::Tile(float x, float y, float width, float height, const char* image_path) 
    : x(x), y(y), tile_width(width), tile_height(height) // 初始化成員變量
{
    shape.reset(new Rectangle(x, y, x + width, y + height));

    // 載入圖片
    ImageCenter *IC = ImageCenter::get_instance();
    image = IC->get(image_path);
    if (!image) {
        std::cerr << "Error: Failed to load image: " << image_path << std::endl;
        exit(1);
    }
}

// 解構函數，釋放圖片資源
Tile::~Tile() {
    if (image) {
        al_destroy_bitmap(image);
    }
}

void Tile::update() {
    // 方磚不需要更新
    //TODO: Implement this function
    //偵測角色位置或是狀態來跟新方塊的狀態
}


// 繪製方磚的圖片
void Tile::draw() {
    al_draw_scaled_bitmap(
        image,                // 原始圖片
        0, 0,                 // 原始圖片的左上角座標 (sx, sy)
        al_get_bitmap_width(image),    // 原始圖片的寬度
        al_get_bitmap_height(image),   // 原始圖片的高度
        x, y,       // 目標位置的左上角座標
        tile_width, tile_height, // 縮放到的寬度和高度
        0                     // 標誌 (默認為 0)
    );
    //al_draw_bitmap(image, left(), top(), 0);
}
