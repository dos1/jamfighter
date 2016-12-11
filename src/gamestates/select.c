/*! \file empty.c
 *  \brief Empty gamestate.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "../common.h"
#include <math.h>
#include <stdio.h>
#include <libsuperderpy.h>

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_FONT *font;
		int blink_counter;

		int selected1, selected2;

		bool chosen1, chosen2;

		int counter;

		ALLEGRO_BITMAP *menu, *pixelator, *p;

		ALLEGRO_BITMAP *characters[24], *faces[24];

		ALLEGRO_BITMAP *ukulele, *wasolele, *tamburyn, *kazoo, *keyboard;

		struct Character *player;

};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->blink_counter++;

	AnimateCharacter(game, data->player, 1);

	if (data->chosen1 && data->chosen2) {
		data->counter++;
	}
	if (data->counter >= 180) {
		game->data->selected1 = data->selected1;
		game->data->selected2 = data->selected2;
		SwitchCurrentGamestate(game, "fight");
	}

}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.

	al_set_target_bitmap(data->pixelator);

	al_draw_bitmap(data->menu, 0, 0, 0);

	ALLEGRO_BITMAP *weaponl, *weaponr;
	weaponl = data->tamburyn;
	weaponr = data->tamburyn;

	if (data->selected1==22) {
		weaponl = data->wasolele;
	}	else if (data->selected1==2) {
		weaponl = data->ukulele;
	} else if (data->selected1==1) {
		weaponl = data->keyboard;
	} else if (data->selected1==15) {
		weaponl = data->kazoo;
	}

	if (data->selected2==22) {
		weaponr = data->wasolele;
	}	else if (data->selected2==2) {
		weaponr = data->ukulele;
	} else if (data->selected2==1) {
		weaponr = data->keyboard;
	}else if (data->selected2==15) {
		weaponr = data->kazoo;
	}

	al_draw_rotated_bitmap(weaponl, 0, 5, 14, 45- data->player->pos, -0.5, 0);
	al_draw_rotated_bitmap(weaponr, 30, 5, 306, 45- data->player->pos, 0.5, ALLEGRO_FLIP_HORIZONTAL);

	SetCharacterPosition(game, data->player, 5, -2, 0);
	DrawCharacter(game, data->player, al_map_rgb(255,255,255), 0);
	SetCharacterPosition(game, data->player, 288, -2, 0);
	DrawCharacter(game, data->player, al_map_rgb(255,255,255), ALLEGRO_FLIP_HORIZONTAL);

	al_draw_bitmap(data->faces[data->selected1], 0, 5 + data->player->pos, 0);
	al_draw_bitmap(data->faces[data->selected2], 284, 5 + data->player->pos, ALLEGRO_FLIP_HORIZONTAL);


	for (int i=0; i<24; i++) {
		al_draw_bitmap(data->characters[i], 11 + 37*(i%8), 64 + 37*(i/8), 0);
	}

	for (int i=0; i<=8; i++) {
		al_draw_line(11+37*i, 64, 11+37*i, 64+37*3, al_map_rgb(255,255,255), 1);
	}
	for (int i=0; i<=3; i++) {
		al_draw_line(10, 64+37*i, 11+37*8, 64+37*i, al_map_rgb(255,255,255), 1);
	}


	if (data->selected1 == data->selected2) {
		al_draw_rectangle(10+37*(data->selected2%8) + 1, 64+37*(data->selected2/8),
		                  11+37*(data->selected2%8) + 37,   65+37*(data->selected2/8) + 36,
		                  al_map_rgba_f(0,0,1,1), 1);

		al_draw_filled_rectangle(11+37*(data->selected2%8), 65+37*(data->selected2/8),
		                  10+37*(data->selected2%8) + 37,   64+37*(data->selected2/8) + 37,
		                  al_map_rgba_f(0,0,((sin(data->blink_counter/20.0) / 2 + 0.5)) * 0.5,0.5));

	} else {
		al_draw_rectangle(10+37*(data->selected1%8) + 1, 64+37*(data->selected1/8),
		                  11+37*(data->selected1%8) + 37,   65+37*(data->selected1/8) + 36,
		                  al_map_rgba_f(0,1,1,1), 1);

		al_draw_filled_rectangle(11+37*(data->selected1%8), 65+37*(data->selected1/8),
		                  10+37*(data->selected1%8) + 37,   64+37*(data->selected1/8) + 37,
		                  al_map_rgba_f((data->chosen1 ? 1 : (sin(data->blink_counter/20.0) / 2 + 0.5)) * 0.5,1*0.5,1*0.5,0.5));



		al_draw_rectangle(10+37*(data->selected2%8) + 1, 64+37*(data->selected2/8),
		                  11+37*(data->selected2%8) + 37,   65+37*(data->selected2/8) + 36,
		                  al_map_rgba_f(1,0,1,1), 1);

		al_draw_filled_rectangle(11+37*(data->selected2%8), 65+37*(data->selected2/8),
		                  10+37*(data->selected2%8) + 37,   64+37*(data->selected2/8) + 37,
		                  al_map_rgba_f(1*0.5,(data->chosen2 ? 1 : (sin(data->blink_counter/20.0) / 2 + 0.5)) * 0.5,1*0.5,0.5));

	}

	if (data->chosen1) {
		DrawTextWithShadow(data->font, al_map_rgb(255,255,255), 12+37*(data->selected1%8), 66+37*(data->selected1/8), ALLEGRO_ALIGN_LEFT, "1");
	}
	if (data->chosen2) {
		DrawTextWithShadow(data->font, al_map_rgb(255,255,255), 10+37*((data->selected2%8) + 1), 66+37*(data->selected2/8), ALLEGRO_ALIGN_RIGHT, "2");
	}


	al_set_target_backbuffer(game->display);
	al_draw_bitmap(data->pixelator, 0, 0, 0);
}


void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}


	if (!data->chosen1) {

		if ((ev->type==ALLEGRO_EVENT_JOYSTICK_AXIS) && (ev->joystick.id == al_get_joystick(0))) {
			if (ev->joystick.stick==0) {
				if (ev->joystick.axis==0) {
					if (ev->joystick.pos==1) {
						data->selected1 += 1;
					}
					if (ev->joystick.pos==-1) {
						data->selected1 -= 1;
					}
				}
				if (ev->joystick.axis==1) {
					if (ev->joystick.pos==-1) {
						data->selected1 -= 8;
					}
					if (ev->joystick.pos==1) {
						data->selected1 += 8;
					}
				}
			}

		}
		/*
		if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_W)) {
			data->selected1 -= 8;
		}
		if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_S)) {
			data->selected1 += 8;
		}
		if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_D)) {
			data->selected1 += 1;
		}
		if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_A)) {
			data->selected1 -= 1;
		}*/
	}

	if (!data->chosen2) {


		if ((ev->type==ALLEGRO_EVENT_JOYSTICK_AXIS) && (ev->joystick.id == al_get_joystick(1))) {
			if (ev->joystick.stick==0) {
				if (ev->joystick.axis==0) {
					if (ev->joystick.pos==1) {
						data->selected2 += 1;
					}
					if (ev->joystick.pos==-1) {
						data->selected2 -= 1;
					}
				}
				if (ev->joystick.axis==1) {
					if (ev->joystick.pos==-1) {
						data->selected2 -= 8;
					}
					if (ev->joystick.pos==1) {
						data->selected2 += 8;
					}
				}
			}
		}


		/*
		if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_UP)) {
			data->selected2 -= 8;
		}
		if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_DOWN)) {
			data->selected2 += 8;
		}
		if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT)) {
			data->selected2 += 1;
		}
		if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_LEFT)) {
			data->selected2 -= 1;
		}*/
	}

	if ((ev->type==ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.id == al_get_joystick(0))) {
		if ((!data->chosen1) || (!data->chosen2)) {
			char *filename = malloc(32*sizeof(char));
			snprintf(filename, 20, "calls/%d.wav", data->selected1);
			if (al_filename_exists(GetDataFilePathNull(game, filename))) {
				ALLEGRO_SAMPLE *sample = al_load_sample(GetDataFilePath(game, filename));
				ALLEGRO_SAMPLE_INSTANCE *sound = al_create_sample_instance(sample);
				al_attach_sample_instance_to_mixer(sound, game->audio.voice);
				al_set_sample_instance_playmode(sound, ALLEGRO_PLAYMODE_ONCE);
				al_play_sample_instance(sound);
			}
			free(filename);
		}
		data->chosen1 = ev->joystick.button != 5;
	}
	if ((ev->type==ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.id == al_get_joystick(1))) {
		if ((!data->chosen1) || (!data->chosen2)) {
			char *filename = malloc(32*sizeof(char));
			snprintf(filename, 20, "calls/%d.wav", data->selected2);
			if (al_filename_exists(GetDataFilePathNull(game, filename))) {
				ALLEGRO_SAMPLE *sample = al_load_sample(GetDataFilePath(game, filename));
				ALLEGRO_SAMPLE_INSTANCE *sound = al_create_sample_instance(sample);
				al_attach_sample_instance_to_mixer(sound, game->audio.voice);
				al_set_sample_instance_playmode(sound, ALLEGRO_PLAYMODE_ONCE);
				al_play_sample_instance(sound);
			}
			free(filename);
		}
		data->chosen2 = ev->joystick.button != 5;
	}


	if (data->selected1 < 0) {
		data->selected1 = 24+data->selected1;
	}
	if (data->selected1 > 23) {
		data->selected1 -= 24;
	}
	if (data->selected2 < 0) {
		data->selected2 = 24+data->selected2;
	}
	if (data->selected2 > 23) {
		data->selected2 -= 24;
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->font = al_create_builtin_font();
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->menu = al_load_bitmap(GetDataFilePath(game, "menu.png"));

	data->pixelator = al_create_bitmap(320, 180);

	data->wasolele = al_load_bitmap(GetDataFilePath(game, "wasolele.png"));
	data->ukulele = al_load_bitmap(GetDataFilePath(game, "ukulele.png"));
	data->tamburyn = al_load_bitmap(GetDataFilePath(game, "tamburyn.png"));
	data->kazoo = al_load_bitmap(GetDataFilePath(game, "kazoo.png"));
	data->keyboard = al_load_bitmap(GetDataFilePath(game, "keyboard.png"));

	for (int i=0; i<24; i++) {
		char *name = malloc(32*sizeof(char));
		snprintf(name, 32, "tiles/%d.png", i);
		data->characters[i] = al_load_bitmap(GetDataFilePath(game, name));
		snprintf(name, 32, "faces/%d.png", i);
		data->faces[i] = al_load_bitmap(GetDataFilePath(game, name));
		free(name);
	}

	data->player = CreateCharacter(game, "player");
	RegisterSpritesheet(game, data->player, "stand");
	LoadSpritesheets(game, data->player);
	SelectSpritesheet(game, data->player, "stand");

	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->blink_counter = 0;
	data->selected1 = 0;
	data->selected2 = 23;
	data->chosen1 = false;
	data->chosen2 = false;
	data->counter = 0;
}

void Gamestate_Stop(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
}

void Gamestate_Pause(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic not ProcessEvent)
	// Pause your timers here.
}

void Gamestate_Resume(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers here.
}

// Ignore this for now.
// TODO: Check, comment, refine and/or remove:
void Gamestate_Reload(struct Game *game, struct GamestateResources* data) {}
