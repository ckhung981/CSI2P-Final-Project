#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED
#include "Object.h"
#include <string>
#include <allegro5/allegro.h>
class Tile : public Object 
{
    public:
        // 構造函數 (建構子)
        Tile(float x, float y, float width, float height, const char* image_path);

        // 解構函數 (解構子)
        ~Tile();

        // 繪製方磚
        void draw();

        // 更新方磚
        void update();

        // 方塊的邊界
        float top() const { return shape->center_y() - tile_height / 2; }
        float bottom() const { return shape->center_y() + tile_height / 2; }
        float left() const { return shape->center_x() - tile_width / 2; }
        float right() const { return shape->center_x() + tile_width / 2; }

    private:
        ALLEGRO_BITMAP *image; // 方磚的圖片
        float tile_width;  // 這是寬度
        float tile_height; // 這是高度
};


#endif // TILE_H_INCLUDED