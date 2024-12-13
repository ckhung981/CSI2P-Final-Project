#ifndef HERO_H_INCLUDED
#define HERO_H_INCLUDED

#include "Object.h"
#include "Map.h"
#include <map>
#include <string>
enum class HeroState{
    LEFT, 
    RIGHT, 
    FRONT, 
    BACK,
    HEROSTATE_MAX
};

class Hero : public Object
{
    public:
        void init();
        void update();
        void draw();
        void die();
    public:
        int HP;
        float hero_width;
        float hero_height;
        // ====== 物理系統參數 ======
        float velocity_y;// 垂直速度
        float gravity ;// 重力加速度 (可以根據需要調整)
        float jump_speed;// 跳躍時的初速度 
        const int max_jumps = 2; // 最大跳躍次數
    private:
        
        HeroState state = HeroState::FRONT;
        double speed = 5;
        std::map<HeroState, std::string> gif_path;
};


#endif // HERO_H_INCLUDED