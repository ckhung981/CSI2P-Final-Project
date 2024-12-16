#ifndef DATACENTER_H_INCLUDED
#define DATACENTER_H_INCLUDED

#include <map>
#include <vector>
#include <allegro5/keycodes.h>
#include <allegro5/mouse.h>
#include "../shapes/Point.h"

class Player;
class Level;
class Monster;
class Tower;
class Bullet;
class Tile;
class Map;
class Hero;
class Spike;
class Portal;

/**
 * @brief Stores generic global data and relatively small data structures.
 * @details The globally used data such as FPS (frames per second), windows size, game region, and states of input devices (mouse and keyboard).
 * Player and Level are stored here mainly because the objects are small enough that do not require complex management.
 * Other specific data like game objects (towers, monsters ... etc) or ALLEGRO_BITMAP will be managed by other center objects.
 */
class DataCenter
{
public:
	static DataCenter *get_instance() {
		static DataCenter DC;
		return &DC;
	}
	~DataCenter();
	void reset();
	void remove_tile();
public:
	bool debug_mode = false;
	bool invincible = false;
	bool is_win = false;
	bool is_in_start = true;
	int death_count = 0;
	double scale;
	double FPS;
	int window_width, window_height;
	/**
	 * @brief The width and height of game area (not window size). That is, the region excludes menu region.
	 * @details The game area is sticked to the top-left of the display window.
	 */
	int game_field_length;
	/**
	 * @brief Stores the keyboard state whether a key is being pressed.
	 * @details The states will be updated once a key is pressed, asynchronously with frame update.
	 * @see Game::execute()
	 */
	bool key_state[ALLEGRO_KEY_MAX];
	/**
	 * @brief Stores the keyboard states of the previous frame.
	 * @details The states will be updated every frame.
	 * @see Game::game_update()
	 */
	bool prev_key_state[ALLEGRO_KEY_MAX];
	/**
	 * @brief Stores the mouse position relative to the game window.
	 * @details The state will be update once the mouse is moved.
	 * @see Game::execute()
	 */
	Point mouse;
	/**
	 * @brief Stores the state of every mouse button.
	 * @details The state will be update once a mouse button is clicked.
	 * @see Game::execute()
	 */
	bool mouse_state[ALLEGRO_MOUSE_MAX_EXTRA_AXES];
	/**
	 * @brief Stores the state of every mouse button of the previous frame.
	 * @details The state will be update every frame.
	 * @see Game::game_update()
	 */
	bool prev_mouse_state[ALLEGRO_MOUSE_MAX_EXTRA_AXES];
public:
	/**
	 * @brief Stores the basic information that a player should have.
	 * @details For a tower-defense game, coin and health point is enough to represent a player.
	 * @see Player
	 */
	Player *player;
	/**
	 * @brief Loads and stores the information of a level.
	 * @see Level
	 */
	Level *level;
	/**
	 * @brief Raw list of Monster objects.
	 * @see Monster
	 */ 
	std::vector<Monster*> monsters;
	/**
	 * @brief Raw list of Tower objects.
	 * @see Tower
	 */
	std::vector<Tower*> towers;
	/**
	 * @brief Raw list of Bullet objects.
	 * @see Bullet
	 */
	std::vector<Bullet*> towerBullets;

	std::vector<Tile*> tiles;
	/**
	 * @brief Stores the map of the game.
	 * @details The map is a 2D array of tiles.
	 * @see Tile
	 */
	std::vector<Spike*> spikes;
	/**
	 * @brief Stores the spikes of the game.
	 * @details The spikes are the obstacles that the hero should avoid.
	 * @see Spike
	 */
	std::vector<Portal*> portals;
	/**
	 * @brief Stores the portals of the game.
	 * @details The portals are the objects that the hero should reach to win the game.
	 * @see Portal
	 */
	

	Map *map;
	Hero *hero;
	/**
	 * @brief Stores the hero object.
	 * @details The hero is the main character of the game.
	 * @see Hero
	 */


private:
	DataCenter();
};

#endif
