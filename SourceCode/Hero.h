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
    private:
        
        float hero_width;
        float hero_height;
        HeroState state = HeroState::FRONT;
        double speed = 5;
        std::map<HeroState, std::string> gif_path;
};


#endif // HERO_H_INCLUDED