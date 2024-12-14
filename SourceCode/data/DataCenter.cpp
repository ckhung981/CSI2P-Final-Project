#include "DataCenter.h"
#include <cstring>
#include <algorithm>
#include <iostream>
#include "../Player.h"
#include "../Hero.h"
#include "../map_items/Tile.h"
#include "../map_items/Spike.h"

// fixed settings
namespace DataSetting {
	constexpr double FPS = 60;
	float origin_window_width = 800;
	constexpr int window_width = 1200;
	constexpr int window_height = 900;
	constexpr int game_field_length = 900;
}

DataCenter::DataCenter() {
	this->FPS = DataSetting::FPS;
	this->window_width = DataSetting::window_width;
	this->window_height = DataSetting::window_height;
	this->game_field_length = DataSetting::game_field_length;
	memset(key_state, false, sizeof(key_state));
	memset(prev_key_state, false, sizeof(prev_key_state));
	mouse = Point(0, 0);
	memset(mouse_state, false, sizeof(mouse_state));
	memset(prev_mouse_state, false, sizeof(prev_mouse_state));
	scale = this->window_width / DataSetting::origin_window_width;
	player = new Player();
	hero = new Hero();
	map = new Map();
}

DataCenter::~DataCenter() {
	reset();
	delete player;
	for(Tile *&t : tiles) {
		delete t;
	}
	for(Spike *&s : spikes) {
		delete s;
	}
	for(Portal *&p : portals) {
		delete p;
	}
	delete map;
	delete hero;
}

void DataCenter::reset() {
	tiles.clear(); // 清除所有 tiles
	spikes.clear(); // 清除所有 spikes
	portals.clear(); // 清除所有 portals
	// 這裡可以添加其他遊戲物件的重置操作
}
void DataCenter::remove_tile() {	// 移除標記為 to_delete 的 tile
    tiles.erase(
        std::remove_if(tiles.begin(), tiles.end(), [](Tile* tile_ptr) {
            if (tile_ptr->to_delete) {
                return true; // 從vector中移除
            }
            return false; // 不刪除這個元素
        }),
        tiles.end() // remove_if 返回新的 vector 末尾
    );
}

		