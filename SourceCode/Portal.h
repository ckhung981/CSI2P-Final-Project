#ifndef PORTAL_H_INCLUDED
#define PORTAL_H_INCLUDED
#include "Object.h"
#include <string>
#include <allegro5/allegro.h>
class Portal : public Object 
{
    public:
        // 構造函數 (建構子)
        Portal(float x, float y, float width, float height, const char* image_path);

        // 解構函數 (解構子)
        ~Portal();

        // 繪製方磚
        void draw();

        // 更新方磚
        void update();

        // 方塊的邊界
        float top() const { return shape->center_y() - portal_height / 2; }
        float bottom() const { return shape->center_y() + portal_height / 2; }
        float left() const { return shape->center_x() - portal_width / 2; }
        float right() const { return shape->center_x() + portal_width / 2; }

        //取得方塊的寬度
        float get_width() const { return portal_width; }

        //取得方塊的高度
        float get_height() const { return portal_height; }
    private:
        ALLEGRO_BITMAP *image; // 物件圖片
        float portal_width;  // 寬度
        float portal_height; // 高度
        float x; // tile x 座標
        float y; // tile y 座標
};


#endif // PORTAL_H_INCLUDED