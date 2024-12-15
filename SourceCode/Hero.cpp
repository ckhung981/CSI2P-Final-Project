#include "Hero.h"
#include "data/GIFCenter.h"
#include "data/DataCenter.h"
#include "shapes/Rectangle.h"
#include <allegro5/allegro_primitives.h>
#include "Map.h"
#include "map_items/Tile.h"
#include <cstdio>
#include <iostream>
#include <unistd.h>
namespace HeroSetting {
	static constexpr char gif_root_path[50] = "./assets/gif/hero";
	static constexpr char gif_postfix[][10] = {
		"left", "right", "front", "back"
	};
}
void Hero::init(){
    for ( size_t type = 0; type < static_cast<size_t>(HeroState::HEROSTATE_MAX); ++type){
        char buffer[50];
        sprintf(buffer, "%s/dragonite_%s.gif",
            HeroSetting::gif_root_path,
            HeroSetting::gif_postfix[static_cast<int>(type)]);
        gif_path[static_cast<HeroState>(type)] = std::string(buffer);    
    }
    DataCenter *DC = DataCenter::get_instance();
    Map *map = DC->map;
    
    if (!DC->tiles.empty()) {
        Tile &tile = *DC->tiles[0];
        this->hero_width = tile.get_width();
        this->hero_height = tile.get_height();
    }
    //ALGIF_ANIMATION *gif = GIFC->get(gif_path[HeroState::FRONT]);

    // 調整 Hero 的寬高與 Tile 的一致
    shape.reset(new Rectangle{
        map->get_hero_x() ,
        map->get_hero_y() ,
        map->get_hero_x() + hero_width ,
        map->get_hero_y() + hero_height
    });
    // ====== 物理系統參數 ======
        velocity_y = 0.0f * DC->scale ;   // 垂直速度
        gravity = 0.5f * DC->scale;       // 重力加速度 (可以根據需要調整)
        jump_speed = -10.0f * DC->scale;  // 跳躍時的初速度 
    // ====== 角色狀態 ======
        HP = 1;

}
void Hero::update() {
    DataCenter *DC = DataCenter::get_instance();
    
/*
    // 取得角色的寬高
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gif_path[HeroState::FRONT]);
    float hero_width = gif->width;
    float hero_height = gif->height;
*/
    // 取得角色的當前座標
    float current_x = shape->center_x();
    float current_y = shape->center_y();

    // 角色的邊界
    float hero_bottom = current_y + hero_height / 2;
    float hero_top = current_y - hero_height / 2;
    float hero_left = current_x - hero_width / 2;
    float hero_right = current_x + hero_width / 2;

    // 取得視窗大小
    float window_width = DC->window_width;
    float window_height = DC->window_height;

    bool on_ground = (current_y + hero_height / 2 >= window_height); // 檢查角色是否在地面
    static int jump_count = 0;      // 記錄跳躍次數
   

    // 角色受重力影響，下落速度增加
    velocity_y += gravity;
    shape->update_center_y(current_y + velocity_y);

    float hit_buffet_y = abs(velocity_y*1.1);
    float hit_buffet_x = abs(speed*1.1);
    // 垂直碰撞檢測
    for (const auto &tile_ptr : DC->tiles) {
        Tile &tile = *tile_ptr;
        if (shape->overlap(*tile.shape)) {
            // 如果角色與 Tile 頂部發生垂直碰撞
            if (hero_bottom >= tile.top() &&        // 角色底部接觸 Tile 頂部
                hero_bottom <= tile.top() + hit_buffet_y && // 角色底部未超過 Tile 頂部
                hero_top < tile.top() &&            // 角色頂部未超過 Tile 頂部
                hero_right > tile.left() &&         // 角色的右側超過 Tile 的左側
                hero_left < tile.right()) {         // Tile 的右側超過角色的左側
                shape->update_center_y(tile.top() - hero_height / 2);
                velocity_y = 0;
                on_ground = true;
                break;
            } 
            // 如果角色與 Tile 底部發生垂直碰撞
            if (hero_bottom > tile.bottom() &&        // 角色底部未超過 Tile 底部
                hero_top <= tile.bottom() &&            // 角色頂部接觸 Tile 底部
                hero_top >= tile.bottom() - hit_buffet_y && // 角色頂部未超過 Tile 底部
                hero_right > tile.left() &&         // 角色的右側超過 Tile 的左側
                hero_left < tile.right()) {         // Tile 的右側超過角色的左側
                shape->update_center_y(tile.bottom() + hero_height / 2 + 0.5); // 將角色鎖定在 Tile 上 // 0.5 是為了防止碰撞重複偵測
                velocity_y = 0; // 停止速度，角色停止  下落
                break;
            }
        }
    }

    // 如果 Hero 掉到地面
    if (shape->center_y() + hero_height / 2 >= window_height) {
        // Hero回到地面，防止穿透
        die();
    }
 
    // 如果角色在平面上，重置跳躍次數
    if (on_ground) {
        velocity_y = 0; // 停止速度，角色停止下落
        jump_count = 0; // 重置跳躍次數，因為已經回到地面
    }

    // 如果玩家按下空白鍵，並且還有跳躍次數，則進行跳躍
    if ( DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE] && jump_count < max_jumps) {
        velocity_y = jump_speed; // 給角色一個向上的速度
        jump_count++; // 增加跳躍次數
    }


    // 水平方向碰撞檢測（處理移動）
    if (DC->key_state[ALLEGRO_KEY_LEFT]) {
        if (current_x - speed - hero_width / 2 > 0) {
            shape->update_center_x(current_x - speed);
            // 碰撞檢測
            for (const auto &tile_ptr : DC->tiles) {
                Tile &tile = *tile_ptr;
                if (shape->overlap(*tile.shape)) {
                    // 如果角色與 Tile 右側發生水平碰撞
                    if (hero_left <= tile.right() &&         // 角色的左側未超過 Tile 的右側
                        hero_left >= tile.right() - hit_buffet_x && // 角色的左側未超過 Tile 的右側
                        hero_right > tile.right() &&        // 角色的右側超過 Tile 的右側
                        hero_bottom > tile.top() &&         // 角色的底部超過 Tile 的頂部
                        hero_top < tile.bottom()) {         // 角色的頂部未超過 Tile 的底部
                        shape->update_center_x(tile.right() + hero_width / 2);
                        break;
                    }
                }
            }
            
            state = HeroState::LEFT;
        }
    }
    if (DC->key_state[ALLEGRO_KEY_RIGHT]) {
        if (current_x + speed + hero_width / 2 < window_width) {
            shape->update_center_x(current_x + speed);
            // 碰撞檢測
            for (const auto &tile_ptr : DC->tiles) {
                Tile &tile = *tile_ptr ;
                if (shape->overlap(*tile.shape)) {
                    // 如果角色與 Tile 左側發生水平碰撞
                    if (hero_right >= tile.left() &&         // 角色的右側超過 Tile 的左側
                        hero_right <= tile.left() + hit_buffet_x && // 角色的右側未超過 Tile 的左側
                        hero_left < tile.left() &&          // 角色的左側未超過 Tile 的左側
                        hero_bottom > tile.top() &&         // 角色的底部超過 Tile 的頂部
                        hero_top < tile.bottom()) {         // 角色的頂部未超過 Tile 的底部
                        shape->update_center_x(tile.left() - hero_width / 2);
                        break;
                    }
                }
            }
            state = HeroState::RIGHT;
        }
    }
}


void Hero::draw(){
    GIFCenter *GIFC = GIFCenter::get_instance();
    DataCenter *DC = DataCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gif_path[state]);
    /*
	algif_draw_gif(
		gif,
		shape->center_x() - gif->width / 2,
		shape->center_y() - gif->height / 2,
        0);
*/
    // 縮放的目標大小
    float target_width = this->hero_width;
    float target_height = this->hero_height;
    // 使用 Allegro 繪製動態縮放的 GIF
    al_draw_scaled_bitmap(
        gif->frames[gif->display_index].rendered, // GIF 當前幀的圖片
        0, 0, // 源圖的左上角（x, y）
        gif->width, // 原始寬
        gif->height, // 原始高
        shape->center_x() - target_width / 2, // 目標繪製的左上角 X 坐標
        shape->center_y() - target_height / 2, // 目標繪製的左上角 Y 坐標
        target_width,  // 目標寬度
        target_height, // 目標高度
        0 // 無特殊標誌
    );
    //==================================================================================================
    //for debug
    if (DC->debug_mode) {
        // 繪製邊框 (以矩形為例)
        al_draw_rectangle(
            shape->center_x() - this->hero_width / 2, 
            shape->center_y() - this->hero_height / 2,
            shape->center_x() + this->hero_width / 2, 
            shape->center_y() + this->hero_height / 2,
            al_map_rgb(255, 0, 0), // 紅色邊框
            2                      // 邊框寬度
        );
    }
    //==================================================================================================
}    

void Hero::die(){
    //執行角色死亡的動作
    HP = 0;
    //播放死亡動畫


}