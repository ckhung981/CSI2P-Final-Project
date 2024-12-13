#ifndef SPIKE_H_INCLUDED
#define SPIKE_H_INCLUDED
#include "Object.h"
#include <string>
#include <allegro5/allegro.h>

class Spike : public Object 
{
    public:
        // 構造函數 (建構子)
        Spike(float x, float y, float width, float height, const char* image_path, int type);

        // 解構函數 (解構子)
        ~Spike();

        // 繪製尖刺
        void draw();

        // 更新尖刺
        void update();

        //取得方塊的寬度
        float get_width() const { return tile_width; }

        //取得方塊的高度
        float get_height() const { return tile_height; }
    private:
        ALLEGRO_BITMAP *image; // 尖刺的圖片
        float tile_width;  // 這是寬度
        float tile_height; // 這是高度
        float x; // tile x 座標
        float y; // tile y 座標
        int type;
        bool detected;
        bool is_played;
};


#endif // SPIKE_H_INCLUDED