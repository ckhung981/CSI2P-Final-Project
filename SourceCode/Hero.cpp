#include "Hero.h"
#include "data/DataCenter.h"
#include "shapes/Rectangle.h"
#include <allegro5/allegro_primitives.h>
#include "Map.h"
#include "map_items/Tile.h"
#include <cstdio>
#include <iostream>
#include <unistd.h>

namespace HeroSetting {
    constexpr char Hero_idle_image_path[] = "./assets/image/main_character/Kid_idle.png";
    constexpr char Hero_run_image_path[][50] = {
        "./assets/image/main_character/Kid_part_1.png", 
        "./assets/image/main_character/Kid_part_2.png", 
        "./assets/image/main_character/Kid_part_3.png", 
        "./assets/image/main_character/Kid_part_4.png", 
        "./assets/image/main_character/Kid_part_5.png"
    };
    constexpr char Hero_jump_image_path[][50] = {
        "./assets/image/main_character/Kid_jump_u.png",
        "./assets/image/main_character/Kid_jump_d.png"
    };
}

void Hero::init(){
    DataCenter *DC = DataCenter::get_instance();
    Map *map = DC->map;
    
    if (!DC->tiles.empty()) {
        Tile &tile = *DC->tiles[0];
        this->hero_width = tile.get_width();
        this->hero_height = tile.get_height();
    }
    
    shape.reset(new Rectangle{
        map->get_hero_x() ,
        map->get_hero_y() ,
        map->get_hero_x() + hero_width ,
        map->get_hero_y() + hero_height
    });
    frame_index = 0; 
    velocity_y = 0.0f * DC->scale ;   
    gravity = 0.5f * DC->scale;       
    jump_speed = -10.0f * DC->scale;  
    HP = 1;
}

void Hero::update() {
    DataCenter *DC = DataCenter::get_instance();
    
    float current_x = shape->center_x();
    float current_y = shape->center_y();

    float hero_bottom = current_y + hero_height / 2;
    float hero_top = current_y - hero_height / 2;
    float hero_left = current_x - hero_width / 2;
    float hero_right = current_x + hero_width / 2;

    float window_width = DC->window_width;
    float window_height = DC->window_height;

    bool on_ground = (current_y + hero_height / 2 >= window_height);
    static int jump_count = 0;     
    
    velocity_y += gravity;
    shape->update_center_y(current_y + velocity_y);

    float hit_buffet_y = abs(velocity_y*1.1);
    float hit_buffet_x = abs(speed*1.1);
    
    for (const auto &tile_ptr : DC->tiles) {
        Tile &tile = *tile_ptr;
        if (shape->overlap(*tile.shape)) {
            if (hero_bottom >= tile.top() &&        
                hero_bottom <= tile.top() + hit_buffet_y && 
                hero_top < tile.top() &&            
                hero_right > tile.left() &&         
                hero_left < tile.right()) {         
                shape->update_center_y(tile.top() - hero_height / 2);
                velocity_y = 0;
                on_ground = true;
                break;
            } 
            if (hero_bottom > tile.bottom() &&       
                hero_top <= tile.bottom() &&        
                hero_top >= tile.bottom() - hit_buffet_y && 
                hero_right > tile.left() &&         
                hero_left < tile.right()) {         
                shape->update_center_y(tile.bottom() + hero_height / 2 + 0.5);
                velocity_y = 0; 
                break;
            }
        }
    }

    if (shape->center_y() + hero_height / 2 >= window_height) {
        die();
    }
 
    if (on_ground) {
        velocity_y = 0; 
        jump_count = 0; 
    }

    if ( DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE] && jump_count < max_jumps) {
        velocity_y = jump_speed; 
        jump_count++; 
    }

    if (DC->key_state[ALLEGRO_KEY_LEFT] || DC->key_state[ALLEGRO_KEY_RIGHT]) {
        frame_index = (frame_index + 1) % 5; 
    }

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
    DataCenter *DC = DataCenter::get_instance();
    
    ALLEGRO_BITMAP *image;
    if (velocity_y != 0) {
        image = (velocity_y < 0) ? al_load_bitmap(HeroSetting::Hero_jump_image_path[0]) : al_load_bitmap(HeroSetting::Hero_jump_image_path[1]);
    } else if (DC->key_state[ALLEGRO_KEY_LEFT] || DC->key_state[ALLEGRO_KEY_RIGHT]) {
        image = al_load_bitmap(HeroSetting::Hero_run_image_path[frame_index]);
    } else {
        image = al_load_bitmap(HeroSetting::Hero_idle_image_path);
    }
    
    float target_width = this->hero_width;
    float target_height = this->hero_height;
    
    al_draw_scaled_bitmap(
        image, 
        0, 0, 
        al_get_bitmap_width(image), 
        al_get_bitmap_height(image), 
        shape->center_x() - target_width / 2, 
        shape->center_y() - target_height / 2, 
        target_width, 
        target_height, 
        (state == HeroState::RIGHT) ? ALLEGRO_FLIP_HORIZONTAL : 0 
    );
    
    if (DC->debug_mode) {
        al_draw_rectangle(
            shape->center_x() - this->hero_width / 2, 
            shape->center_y() - this->hero_height / 2,
            shape->center_x() + this->hero_width / 2, 
            shape->center_y() + this->hero_height / 2,
            al_map_rgb(255, 0, 0), 
            2 
        );
    }
    al_destroy_bitmap(image);
}    

void Hero::die(){
    DataCenter *DC = DataCenter::get_instance();
    HP = 0;
    if (DC->invincible) {
        std::cout << "Hero died!" << std::endl;
        HP = 1;
    }
}
