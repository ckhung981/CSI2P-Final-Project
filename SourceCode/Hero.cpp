#include "Hero.h"
#include "data/GIFCenter.h"
#include "data/DataCenter.h"
#include "shapes/Rectangle.h"
#include <cstdio>
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
    GIFCenter *GIFC = GIFCenter::get_instance();
    DataCenter *DC = DataCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gif_path[HeroState::FRONT]);
    shape.reset(new Rectangle{
        DC->window_width/2, 
        DC->window_height/2,
        DC->window_width/2 + gif->width,
        DC->window_height/2 + gif->height
});

}
void Hero::update() {
    DataCenter *DC = DataCenter::get_instance();

    // 取得角色的寬高
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gif_path[HeroState::FRONT]);
    float hero_width = gif->width;
    float hero_height = gif->height;

    // 取得角色的當前座標
    float current_x = shape->center_x();
    float current_y = shape->center_y();

    // 取得視窗大小
    float window_width = DC->window_width;
    float window_height = DC->window_height;

    // ====== 物理系統參數 ======
    static float velocity_y = 0.0f;   // 垂直速度
    const float gravity = 0.5f;       // 重力加速度 (可以根據需要調整)
    const float jump_speed = -12.0f;  // 跳躍時的初速度
    bool on_ground = (current_y + hero_height / 2 >= window_height); // 檢查角色是否在地面
    static int jump_count = 0;      // 記錄跳躍次數
    const int max_jumps = 2; // 最大跳躍次數

    // 如果角色在地面上，重置跳躍次數
    if (on_ground) {
        shape->update_center_y(window_height - hero_height / 2); // 鎖定在地面
        velocity_y = 0; // 停止速度，角色停止下落
        jump_count = 0; // 重置跳躍次數，因為已經回到地面
    }

    // 如果玩家按下空白鍵，並且還有跳躍次數，則進行跳躍
    if ( DC->key_state[ALLEGRO_KEY_SPACE] && !DC->prev_key_state[ALLEGRO_KEY_SPACE] && jump_count < max_jumps) {
        velocity_y = jump_speed; // 給角色一個向上的速度
        jump_count++; // 增加跳躍次數
    }

    // 角色受重力影響，下落速度增加
    velocity_y += gravity;

    // 讓角色的 y 座標根據速度進行變化
    shape->update_center_y(current_y + velocity_y);

    // 限制角色不會掉出視窗底部（地面）
    if (shape->center_y() + hero_height / 2 > window_height) {
        shape->update_center_y(window_height - hero_height / 2);
        velocity_y = 0; // 停止速度，角色停止下落
        on_ground = true; // 角色已經回到地面上
    }

    // 按鍵控制角色移動
    if (DC->key_state[ALLEGRO_KEY_LEFT]) {
        if (current_x - speed - hero_width / 2 > 0) {
            shape->update_center_x(current_x - speed);
            state = HeroState::LEFT;
        }
    } 
    else if (DC->key_state[ALLEGRO_KEY_RIGHT]) {
        if (current_x + speed + hero_width / 2 < window_width) {
            shape->update_center_x(current_x + speed);
            state = HeroState::RIGHT;
        }
    }
}


void Hero::draw(){
    GIFCenter *GIFC = GIFCenter::get_instance();
    ALGIF_ANIMATION *gif = GIFC->get(gif_path[state]);
	algif_draw_gif(
		gif,
		shape->center_x() - gif->width / 2,
		shape->center_y() - gif->height / 2,
        0);
        
}