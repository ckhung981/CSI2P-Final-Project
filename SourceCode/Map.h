#ifndef MAP_H
#define MAP_H

#include "map_items/Portal.h"
#include "map_items/Tile.h"
#include "map_items/Spike.h"
#include <vector>

class Map {
	private:
		float hero_x, hero_y;

	public:
		// 構造函數
		Map();

		// 初始化整個地圖
		void init();

		// 繪製所有的方磚
		void draw();

		// 更新整個地圖e
		void update();

		void create_map(const char* map_path);
		void destroy_map();

		Tile  *create_tile(float x, float y, float width, float height, const char* image_path, int type);
		Spike *create_spike(float x, float y, float width, float height, const char* image_path, int type);
		Portal *create_portal(float x, float y, float width, float height, const char* image_path);

		// hero的位置
		float get_hero_x() const { return hero_x; }
		float get_hero_y() const { return hero_y; }
};

#endif // MAP_H