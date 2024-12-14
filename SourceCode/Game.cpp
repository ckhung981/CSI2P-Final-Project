#include "Game.h"
#include "Utils.h"
#include "data/DataCenter.h"
#include "data/OperationCenter.h"
#include "data/SoundCenter.h"
#include "data/ImageCenter.h"
#include "data/FontCenter.h"
#include "Player.h"
#include "Hero.h"
#include "Map.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <vector>
#include <cstring>
#include <iostream>

// fixed settings
constexpr char game_icon_img_path[] = "./assets/image/game_icon.png";
constexpr char game_start_sound_path[] = "./assets/sound/growl.wav";
constexpr char background_img_path[] = "";
//constexpr char background_sound_path[] = "./assets/sound/BackgroundMusic.ogg";
constexpr char background_sound_path[] = "./assets/sound/BGM.mp3";
constexpr char game_over_sound_path[] = "./assets/sound/die_1.MP3";
constexpr char game_win_sound_path[] = "./assets/sound/win.mp3";

/**
 * @brief Game entry.
 * @details The function processes all allegro events and update the event state to a generic data storage (i.e. DataCenter).
 * For timer event, the game_update and game_draw function will be called if and only if the current is timer.
 */
void
Game::execute() {
	DataCenter *DC = DataCenter::get_instance();
	// main game loop
	bool run = true;
	while(run) {
		// process all events here
		al_wait_for_event(event_queue, &event);
		switch(event.type) {
			case ALLEGRO_EVENT_TIMER: {
				run &= game_update();
				game_draw();
				break;
			} case ALLEGRO_EVENT_DISPLAY_CLOSE: { // stop game
				run = false;
				break;
			} case ALLEGRO_EVENT_KEY_DOWN: {
				DC->key_state[event.keyboard.keycode] = true;
				break;
			} case ALLEGRO_EVENT_KEY_UP: {
				DC->key_state[event.keyboard.keycode] = false;
				break;
			} case ALLEGRO_EVENT_MOUSE_AXES: {
				DC->mouse.x = event.mouse.x;
				DC->mouse.y = event.mouse.y;
				break;
			} case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
				DC->mouse_state[event.mouse.button] = true;
				break;
			} case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
				DC->mouse_state[event.mouse.button] = false;
				break;
			} default: break;
		}
	}
}

/**
 * @brief Initialize all allegro addons and the game body.
 * @details Only one timer is created since a game and all its data should be processed synchronously.
 */
Game::Game() {
	DataCenter *DC = DataCenter::get_instance();
	GAME_ASSERT(al_init(), "failed to initialize allegro.");

	// initialize allegro addons
	bool addon_init = true;
	addon_init &= al_init_primitives_addon();
	addon_init &= al_init_font_addon();
	addon_init &= al_init_ttf_addon();
	addon_init &= al_init_image_addon();
	addon_init &= al_init_acodec_addon();
	GAME_ASSERT(addon_init, "failed to initialize allegro addons.");

	// initialize events
	bool event_init = true;
	event_init &= al_install_keyboard();
	event_init &= al_install_mouse();
	event_init &= al_install_audio();
	GAME_ASSERT(event_init, "failed to initialize allegro events.");

	// initialize game body
	GAME_ASSERT(
		display = al_create_display(DC->window_width, DC->window_height),
		"failed to create display.");
	GAME_ASSERT(
		timer = al_create_timer(1.0 / DC->FPS),
		"failed to create timer.");
	GAME_ASSERT(
		event_queue = al_create_event_queue(),
		"failed to create event queue.");

	debug_log("Game initialized.\n");
	game_init();
}

/**
 * @brief Initialize all auxiliary resources.
 */
void
Game::game_init() {
	DataCenter *DC = DataCenter::get_instance();
	SoundCenter *SC = SoundCenter::get_instance();
	ImageCenter *IC = ImageCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();
	// set window icon
	game_icon = IC->get(game_icon_img_path);
	al_set_display_icon(display, game_icon);

	// register events to event_queue
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

	// init sound setting
	SC->init();

	// init font setting
	FC->init();

	// init map setting
	DC->map->init();

	DC->hero->init();
	

	// game start
	if (std::strlen(background_img_path) > 0) {
		background = IC->get(background_img_path);
	}
	else{
		background = nullptr;
	}
	debug_log("Game state: change to START\n");
	state = STATE::START;
	al_start_timer(timer);
}

/**
 * @brief The function processes all data update.
 * @details The behavior of the whole game body is determined by its state.
 * @return Whether the game should keep running (true) or reaches the termination criteria (false).
 * @see Game::STATE
 */
bool
Game::game_update() {
	DataCenter *DC = DataCenter::get_instance();
	OperationCenter *OC = OperationCenter::get_instance();
	SoundCenter *SC = SoundCenter::get_instance();
	static ALLEGRO_SAMPLE_INSTANCE *background = nullptr;
	static ALLEGRO_SAMPLE_INSTANCE *gameover = nullptr;
	static ALLEGRO_SAMPLE_INSTANCE *win = nullptr;

	switch(state) {
		case STATE::START: {
			
			debug_log("<Game> state: change to LEVEL\n");
			state = STATE::LEVEL;
			
			break;
		} case STATE::LEVEL: {
			static bool BGM_played = false;
			if(!BGM_played) {
				background = SC->play(background_sound_path, ALLEGRO_PLAYMODE_LOOP);
				BGM_played = true;
			}

			if(DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
				SC->toggle_playing(background);
				debug_log("<Game> state: change to PAUSE\n");
				state = STATE::PAUSE;
			}
			/*
			if(DC->level->remain_monsters() == 0 && DC->monsters.size() == 0) {
				debug_log("<Game> state: change to END\n");
				state = STATE::END;
			}*/
			if(DC->hero->HP == 0) {
				debug_log("<Game> state: change to DIE\n");
				DC->death_count++;
				state = STATE::DIE;
			}

			if (DC->is_win) {
				debug_log("<Game> state: change to END\n");

				state = STATE::WIN;
			}

			break;
		} case STATE::PAUSE: {
			if(DC->key_state[ALLEGRO_KEY_P] && !DC->prev_key_state[ALLEGRO_KEY_P]) {
				SC->toggle_playing(background);
				debug_log("<Game> state: change to LEVEL\n");
				state = STATE::LEVEL;
			}
			break;
		} case STATE::DIE: {
			static bool is_die_music_played = false; 
			if (!is_die_music_played) {
				SC->toggle_playing(background); // 停止背景音樂
				gameover = SC->play(game_over_sound_path, ALLEGRO_PLAYMODE_LOOP); // 循環播放死亡音樂
				is_die_music_played = true; 
			}
			if(DC->key_state[ALLEGRO_KEY_R] && !DC->prev_key_state[ALLEGRO_KEY_R]) {
				SC->toggle_playing(gameover);
				SC->toggle_playing(background);
				debug_log("<Game> state: change to START\n");
				game_restart();
				state = STATE::START;
				is_die_music_played = false;
			} else if(DC->key_state[ALLEGRO_KEY_ESCAPE] && !DC->prev_key_state[ALLEGRO_KEY_ESCAPE]) {
				debug_log("<Game> state: change to END\n");
				state = STATE::END;
			}
			break;
		} case STATE::WIN: {
			static bool is_win_music_played = false; 
			if (!is_win_music_played) {
				SC->toggle_playing(background); // 停止背景音樂
				win = SC->play(game_win_sound_path, ALLEGRO_PLAYMODE_LOOP); // 循環播放勝利音樂
				is_win_music_played = true; 
			}
			if (DC->key_state[ALLEGRO_KEY_ESCAPE] && !DC->prev_key_state[ALLEGRO_KEY_ESCAPE]) {
				debug_log("<Game> state: change to END\n");
				state = STATE::END;
			}
			break;
		} case STATE::END: {
			return false;
		}
	}
	// If the game is not paused, we should progress update.
	if(state != STATE::PAUSE ) {
		DC->player->update();
		SC->update();
		if (state !=STATE::DIE) {
			DC->map->update();
			DC->hero->update();
		}
		if(state != STATE::START) {
			OC->update();
		}
	}
	// game_update is finished. The states of current frame will be previous states of the next frame.
	memcpy(DC->prev_key_state, DC->key_state, sizeof(DC->key_state));
	memcpy(DC->prev_mouse_state, DC->mouse_state, sizeof(DC->mouse_state));
	return true;
}

/**
 * @brief Draw the whole game and objects.
 */
void
Game::game_draw() {
	DataCenter *DC = DataCenter::get_instance();
	OperationCenter *OC = OperationCenter::get_instance();
	FontCenter *FC = FontCenter::get_instance();

	// Flush the screen first.
	al_clear_to_color(al_map_rgb(100, 100, 100));
	if(state != STATE::END) {
		// background

		if (background) {
			al_draw_bitmap(background, 0, 0, 0);
		} else {
			al_clear_to_color(al_map_rgb(173, 216, 230)); // 預設背景顏色
		}
		// user interface
		if(state != STATE::START) {
			DC->map->draw();
			DC->hero->draw();
		}
	}
	switch(state) {
		case STATE::START: {
		} case STATE::LEVEL: {
			char buffer[100];
			sprintf(buffer, "Death count = %d", DC->death_count);
			al_draw_text(
				FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
				0, 0,
				ALLEGRO_ALIGN_LEFT, buffer);
			break;
		} case STATE::PAUSE: {
			// game layout cover
			al_draw_filled_rectangle(0, 0, DC->window_width, DC->window_height, al_map_rgba(50, 50, 50, 64));
			al_draw_text(
				FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
				DC->window_width/2., DC->window_height/2.,
				ALLEGRO_ALIGN_CENTRE, "GAME PAUSED (Press P to resume)");
			break;
		} case STATE::DIE: {
			ImageCenter *IC = ImageCenter::get_instance();
			ALLEGRO_BITMAP *image = IC->get("./assets/image/game_over_no_background.png");
			// 計算繪製位置，將其置中
			float target_width = DC->window_width * 0.8;  // 讓圖片寬度為視窗的 80%
			float target_height = target_width * (static_cast<float>(al_get_bitmap_height(image)) / al_get_bitmap_width(image)); // 按比例縮放高度

			float x = (DC->window_width - target_width) / 2; // X 軸置中
			float y = (DC->window_height - target_height) / 2; // Y 軸置於視窗上方三分之一處
			al_draw_scaled_bitmap(
				image,                // 原始圖片
				0, 0,                 // 原始圖片的左上角座標 (sx, sy)
				al_get_bitmap_width(image),    // 原始圖片的寬度
				al_get_bitmap_height(image),   // 原始圖片的高度
				x, y,       // 目標位置的左上角座標
				target_width, target_height, // 縮放到的寬度和高度
				0                     // 標誌 (默認為 0)
			);
			char buffer[100];
			sprintf(buffer, "Death count = %d", DC->death_count);
			al_draw_text(
				FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
				0, 0,
				ALLEGRO_ALIGN_LEFT, buffer);
			break;
		} case STATE::WIN: {
			ImageCenter *IC = ImageCenter::get_instance();
			ALLEGRO_BITMAP *image = IC->get("./assets/image/win.png");
			// 計算繪製位置，將其置中
			float target_width = DC->window_width * 0.8;  // 讓圖片寬度為視窗的 80%
			float target_height = target_width * (static_cast<float>(al_get_bitmap_height(image)) / al_get_bitmap_width(image)); // 按比例縮放高度

			float x = (DC->window_width - target_width) / 2; // X 軸置中
			float y = (DC->window_height - target_height) / 2; // Y 軸置於視窗上方三分之一處
			al_draw_scaled_bitmap(
				image,                // 原始圖片
				0, 0,                 // 原始圖片的左上角座標 (sx, sy)
				al_get_bitmap_width(image),    // 原始圖片的寬度
				al_get_bitmap_height(image),   // 原始圖片的高度
				x, y,       // 目標位置的左上角座標
				target_width, target_height, // 縮放到的寬度和高度
				0                     // 標誌 (默認為 0)
			);
			char buffer[100];
			sprintf(buffer, "Death count = %d", DC->death_count);
			al_draw_text(
				FC->caviar_dreams[FontSize::LARGE], al_map_rgb(255, 255, 255),
				0, 0,
				ALLEGRO_ALIGN_LEFT, buffer);
			break;

			break;	
		} case STATE::END: {
		}
	}
	al_flip_display();
}

void Game::game_restart() {
	DataCenter *DC = DataCenter::get_instance();
	DC->reset();
	DC->hero->init();
	DC->map->init();
	DC->hero->HP = 1;
}


Game::~Game() {
	al_destroy_display(display);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);
}
