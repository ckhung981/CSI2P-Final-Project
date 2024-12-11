#ifndef MAP_H
#define MAP_H

#include "Tile.h"
#include <vector>

class Map {
	private:

	public:
		// 構造函數
		Map();

		// 初始化整個地圖
		void init();

		// 繪製所有的方磚
		void draw();

		// 更新整個地圖
		void update();
	
};

#endif // MAP_H