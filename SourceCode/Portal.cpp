#include "Portal.h"
#include "Hero.h"
#include "data/DataCenter.h"
#include "data/ImageCenter.h"
#include "shapes/Rectangle.h"
#include <allegro5/allegro_primitives.h>
#include <iostream>
#include <cstdio>

// 構造函數 (初始化位置、大小和載入圖片)
Portal::Portal(float x, float y, float width, float height, const char* image_path) 
    : x(x), y(y), portal_width(width), portal_height(height) // 初始化成員變量
{
    shape.reset(new Rectangle{
        x, y,
        x + width, y + height
    });

    // 載入圖片
    ImageCenter *IC = ImageCenter::get_instance();
    image = IC->get(image_path);
    if (!image) {
        std::cerr << "Error: Failed to load image: " << image_path << std::endl;
        exit(1);
    }
}

// 解構函數，釋放圖片資源
Portal::~Portal() {
    if (image) {
        al_destroy_bitmap(image);
    }
}


void Portal::update() {
    //偵測角色碰撞
    DataCenter *DC = DataCenter::get_instance();
    if (shape->overlap(*DC->hero->shape)) {
        DC->is_win = true;
    }
}

// 繪製圖片
void Portal::draw() {

    al_draw_scaled_bitmap(
        image,                // 原始圖片
        0, 0,                 // 原始圖片的左上角座標 (sx, sy)
        al_get_bitmap_width(image),    // 原始圖片的寬度
        al_get_bitmap_height(image),   // 原始圖片的高度
        x, y,       // 目標位置的左上角座標
        portal_width, portal_height, // 縮放到的寬度和高度
        0                     // 標誌 (默認為 0)
    );
    //al_draw_bitmap(image, left(), top(), 0);
    
    //==================================================================================================
    //for debug
    DataCenter *DC = DataCenter::get_instance();
    if (DC->debug_mode) {
        // 繪製方磚邊框
        al_draw_rectangle(
            left(), top(), right(), bottom(),
            al_map_rgb(255, 0, 0), // 邊框顏色 (紅色)
            2                      // 邊框寬度
        );
    }
    //==================================================================================================
    
}
 

