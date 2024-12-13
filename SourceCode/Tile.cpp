#include "Tile.h"
#include "Hero.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "shapes/Rectangle.h"
#include <iostream>
#include <cstdio>

// 構造函數 (初始化位置、大小和載入圖片)
Tile::Tile(float x, float y, float width, float height, const char* image_path, int type) 
    : x(x), y(y), tile_width(width), tile_height(height), type(type)// 初始化成員變量
{
    shape.reset(new Rectangle(x, y, x + width, y + height));


    // 載入圖片
    ImageCenter *IC = ImageCenter::get_instance();
    image = IC->get(image_path);
    if (!image) {
        std::cerr << "Error: Failed to load image: " << image_path << std::endl;
        exit(1);
    }

    if (type == 1) show = true;
    else if (type == 4) show = false;   
    else if (type == 5) show = true;
    else if (type == 6) show = true;
}

// 解構函數，釋放圖片資源
Tile::~Tile() {
    if (image) {
        al_destroy_bitmap(image);
    }
}

void Tile::update() {
    DataCenter *DC = DataCenter::get_instance();
    if (type == 1) return;
    else if (type == 4) // 碰到這個方塊就會顯示
    {
        if (shape->overlap(*DC->hero->shape)) {
            show = true; 
        }
    }
    else if (type == 5) // 碰到這個方塊就會消失
    {
        if (shape->overlap(*DC->hero->shape)) {
            show = false;
            to_delete = true;
        }
    }
    else if (type == 6) return;

}


// 繪製方磚的圖片
void Tile::draw() {
    if (show==true) {
        al_draw_scaled_bitmap(
            image,                // 原始圖片
            0, 0,                 // 原始圖片的左上角座標 (sx, sy)
            al_get_bitmap_width(image),    // 原始圖片的寬度
            al_get_bitmap_height(image),   // 原始圖片的高度
            x, y,       // 目標位置的左上角座標
            tile_width, tile_height, // 縮放到的寬度和高度
            0                     // 標誌 (默認為 0)
        );
    }
    //al_draw_bitmap(image, left(), top(), 0);
}
