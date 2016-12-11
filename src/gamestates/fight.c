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

		int health1, health2;

		int bamcount, bamx, bamy;

		int wins1, wins2;

		ALLEGRO_BITMAP *bg, *pixelator, *bam1, *bam2, *bam;

		ALLEGRO_BITMAP *characters[24], *faces[24];

		ALLEGRO_BITMAP *ukulele, *wasolele, *tamburyn;

		struct Character *player1, *player2, *gandalf;


		float posx1, posy1, posx2, posy2;
		float rot1, rot2;
		bool left1, left2;
		bool walking1, walking2;

		bool colliding1, colliding2;

		ALLEGRO_BITMAP *collider, *colplayer, *colweapon, *gand;

		ALLEGRO_SAMPLE *bamsample[8];
		ALLEGRO_SAMPLE_INSTANCE *bamsound[8];

		ALLEGRO_SAMPLE *roundsample[3], *readysample, *gosample;
		ALLEGRO_SAMPLE_INSTANCE *roundsound[3], *readysound, *gosound;


		int timer;
		int timercount;

		bool started;

		int round;
		int readiness;

		char* text;

};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void EndRound(struct Game *game, struct GamestateResources* data) {

	if (data->health1 == data->health2) {
		return;
	}

	if (data->health1 > data->health2) {
		data->wins1++;
	} else {
		data->wins2++;
	}

	if ((data->wins1==2) || (data->wins2==2)) {
		if (data->wins1==2) {
			game->data->winner = game->data->selected1;
		} else {
			game->data->winner = game->data->selected2;
		}
		SwitchCurrentGamestate(game, "select");
	}

	data->blink_counter = 0;
	data->counter = 0;

	data->colliding1 = false;
	data->colliding2 = false;
data->text = NULL;
data->readiness= 0;
data->round++;

  data->posx1 = 10;
	data->posy1 = 70;

	data->posx2 = 280;
	data->posy2 = 80;

	data->rot1 = 0;
	data->rot2 = 3.14;

	data->left1 = false;
	data->left2 = true;

	data->walking1 = false;
	data->walking2 = false;

	data->health1 = 100;
	data->health2 = 100;

	data->bamcount = 0;

	data->started = false;
	data->timer = 60;
	data->timercount = 0;

}

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->blink_counter++;


	if (data->started) {
		data->timercount++;
		if (data->timer > -1) {
			if (data->timercount==60) {
				data->timer--;
				data->timercount=0;
			}
		} else {
			EndRound(game, data);
		}
	} else {
		if (data->readiness == 0) {
			al_play_sample_instance(data->roundsound[data->round]);
			data->text = strdup("Round 1");
			data->text[strlen(data->text)-1]+=data->round;
		} else if (data->readiness==120) {
			al_play_sample_instance(data->readysound);
			data->text = "READY";
		} else if (data->readiness==200) {
			al_play_sample_instance(data->gosound);
			data->started = true;
			data->text = NULL;
		}
		data->readiness++;
	}

	if ((data->health1 <= 0) || (data->health2 <= 0)) {
		EndRound(game, data);
	}


	bool oldwalking1 = data->walking1;
	bool oldwalking2 = data->walking2;
	bool oldcolliding1 = data->colliding1;
	bool oldcolliding2 = data->colliding2;

	bool moving1 = false;
	bool moving2 = false;

	int bamx = 0;
	int bamy = 0;

	AnimateCharacter(game, data->player1, 1);
	AnimateCharacter(game, data->player2, 1);

	AnimateCharacter(game, data->gandalf, 1);
	ALLEGRO_JOYSTICK_STATE state;
	al_get_joystick_state(al_get_joystick(0), &state);

	data->walking1 = false;

	if (fabs(state.stick[0].axis[0])>=0.2) {
		data->posx1 += state.stick[0].axis[0] * 3;
		data->walking1 = true;
		moving1 = true;
		if (state.stick[0].axis[0]<0) {
			data->left1 = true;
		}
		if (state.stick[0].axis[0]>0) {
			data->left1 = false;
		}
	}
	if (fabs(state.stick[0].axis[1])>=0.5) {
		data->walking1 = true;
		moving1 = true;
		data->posy1 += state.stick[0].axis[1] * 3;
	}

	if ((fabs(state.stick[2].axis[0])>=0.5) || (fabs(state.stick[1].axis[1])>=0.5)) {
		data->rot1 = atan2(-state.stick[2].axis[0],state.stick[1].axis[1]);
		moving1 = true;
	}

	al_get_joystick_state(al_get_joystick(1), &state);

	data->walking2 = false;

	if (fabs(state.stick[0].axis[0])>=0.2) {
		data->posx2 += state.stick[0].axis[0] * 3;
		data->walking2 = true;
		moving2 = true;
		if (state.stick[0].axis[0]<0) {
			data->left2 = true;
		}
		if (state.stick[0].axis[0]>0) {
			data->left2 = false;
		}
	}
	if (fabs(state.stick[0].axis[1])>=0.5) {
		data->walking2 = true;
		moving2 = true;
		data->posy2 += state.stick[0].axis[1] * 3;
	}

	if ((fabs(state.stick[2].axis[0])>=0.5) || (fabs(state.stick[1].axis[1])>=0.5)) {
		data->rot2 = atan2(-state.stick[2].axis[0],state.stick[1].axis[1]);
		moving2 = true;
	}

	if (data->walking1 && !oldwalking1) {
		SelectSpritesheet(game, data->player1, "walk");
	}
	if (oldwalking1 && !data->walking1) {
		SelectSpritesheet(game, data->player1, "stand");
	}
	if (data->walking2 && !oldwalking2) {
		SelectSpritesheet(game, data->player2, "walk");
	}
	if (oldwalking2 && !data->walking2) {
		SelectSpritesheet(game, data->player2, "stand");
	}

	if (data->posx1 < 0) data->posx1 = 0;
	if (data->posx2 < 0) data->posx2 = 0;
	if (data->posy1 < -10) data->posy1 = -10;
	if (data->posy2 < -10) data->posy2 = -10;
	if (data->posx1 > 300) data->posx1 = 300;
	if (data->posx2 > 300) data->posx2 = 300;
	if (data->posy1 > 110) data->posy1 = 110;
	if (data->posy2 > 110) data->posy2 = 110;


	al_set_target_bitmap(data->collider);
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_rotated_bitmap(data->colweapon, 0, 3, 10 + data->posx1 + (data->left1 ? 7 : 0), data->posy1 + 43- data->player1->pos, -data->rot1, 0);
	al_draw_bitmap(data->colplayer, data->posx2, data->posy2, 0);

	data->colliding1 = false;
	data->colliding2 = false;

	for (int i=0; i<320; i++) {
		for (int j=0; j<180; j++) {
			ALLEGRO_COLOR color = al_get_pixel(data->collider, i, j);
			if (color.r >= 0.5) {
				data->colliding1 = true;
				bamx = i;
				bamy = j;
				break;
			}
		}
	}

	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_rotated_bitmap(data->colweapon, 0, 3, 10 + data->posx2 + (data->left2 ? 7 : 0), data->posy2 + 43- data->player2->pos, -data->rot2, 0);
	al_draw_bitmap(data->colplayer, data->posx1, data->posy1, 0);

	for (int i=0; i<320; i++) {
		for (int j=0; j<180; j++) {
			ALLEGRO_COLOR color = al_get_pixel(data->collider, i, j);
			if (color.r >= 0.5) {
				data->colliding2 = true;
				bamx = i;
				bamy = j;
				break;
			}
		}
	}

	if (data->bamcount>0) data->bamcount--;

	if (data->started) {
		if ((!oldcolliding1) && (moving1) && (data->colliding1)) {
			PrintConsole(game, "COLISION 1");
			data->bamcount = 20;
			data->bamx = bamx;
			data->bamy = bamy;
			data->bam = data->bam1;

			int i = rand()%8;
			al_stop_sample_instance(data->bamsound[i]);
			al_play_sample_instance(data->bamsound[i]);

			data->health2 -= 1;
		}
		if ((!oldcolliding2) && (moving2) && (data->colliding2)) {
			PrintConsole(game, "COLISION 2");
			data->bamcount = 20;
			data->bamx = bamx;
			data->bamy = bamy;
			data->bam = data->bam2;

			int i = rand()%8;
			al_stop_sample_instance(data->bamsound[i]);
			al_play_sample_instance(data->bamsound[i]);

			data->health1 -= 1;
		}
	}

	al_set_target_backbuffer(game->display);
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.

	al_set_target_bitmap(data->pixelator);

	al_draw_bitmap(data->bg, 0, 0, 0);
//	SetCharacterPosition(game, data->gandalf, 211, 71, 0);
	al_set_target_bitmap(data->gand);
	SetCharacterPosition(game, data->gandalf, 0, 0, 0);
	DrawCharacter(game, data->gandalf, al_map_rgb(255,255,255), 0);
	al_set_target_bitmap(data->pixelator);
	al_draw_rotated_bitmap(data->gand, 28/2, 16/2, 211+28/2, 71+16/2, -0.05, 0);

	SetCharacterPosition(game, data->gandalf, 81, 44, 0);
	DrawScaledCharacter(game, data->gandalf, al_map_rgb(255,255,255), 0.5, 0.5, 0);

	ALLEGRO_BITMAP *weaponl, *weaponr;
	weaponl = data->tamburyn;
	weaponr = data->tamburyn;

	if (data->selected1==22) {
		weaponl = data->wasolele;
	}	else if (data->selected1==2) {
		weaponl = data->ukulele;
	}

	if (data->selected2==22) {
		weaponr = data->wasolele;
	}	else if (data->selected2==2) {
		weaponr = data->ukulele;
	}

	al_draw_rotated_bitmap(weaponl, 0, 3, 10 + data->posx1 + (data->left1 ? 7 : 0), data->posy1 + 43- data->player1->pos, -data->rot1, 0);
	SetCharacterPosition(game, data->player1, data->posx1, data->posy1, 0);
	DrawCharacter(game, data->player1, al_map_rgb(255,255,255), data->left1 ? ALLEGRO_FLIP_HORIZONTAL : 0);
	al_draw_bitmap(data->faces[data->selected1], -5+data->posx1, data->posy1 + 7 + data->player1->pos, data->left1 ? ALLEGRO_FLIP_HORIZONTAL : 0);

	al_draw_rotated_bitmap(weaponr, 0, 3, 10 + data->posx2 + (data->left2 ? 7 : 0), data->posy2 + 43- data->player2->pos, -data->rot2 , 0);
	SetCharacterPosition(game, data->player2, data->posx2, data->posy2, 0);
	DrawCharacter(game, data->player2, al_map_rgb(255,255,255), data->left2 ? ALLEGRO_FLIP_HORIZONTAL : 0);
	al_draw_bitmap(data->faces[data->selected2], -5+data->posx2, data->posy2 + 7 + data->player2->pos, data->left2 ? ALLEGRO_FLIP_HORIZONTAL : 0);


	if (data->bamcount) {
		al_draw_bitmap(data->bam, data->bamx-25, data->bamy-25, 0);
	}

	al_draw_filled_rectangle(0, 0, 320, 24, al_map_rgba(64,64,64,64));
	al_draw_filled_rectangle(0, 24, 320, 25, al_map_rgba(0,0,0,64));
	al_draw_scaled_bitmap(data->faces[data->selected1], 0, 0, 37, 37, 2, 2, 20, 20, 0);
	al_draw_scaled_bitmap(data->faces[data->selected2], 0, 0, 37, 37, 320-20-2, 2, 20, 20, ALLEGRO_FLIP_HORIZONTAL);

	al_draw_filled_rectangle(24, 5, 24 + 120 + 2, 5+8, al_map_rgb(0,0,0));
	al_draw_filled_rectangle(25, 6, 25 + 120 * data->health1 / 100.0, 6+6, al_map_rgb(200,200,200));

	al_draw_filled_rectangle(320 - 24 - 120 - 2, 5, 320 - 24, 5+8, al_map_rgb(0,0,0));
	al_draw_filled_rectangle(320 - 25 - 120 * data->health2 / 100.0, 6, 320 - 25, 6+6, al_map_rgb(200,200,200));

	al_draw_filled_rectangle(24, 5+8+2, 24 + 4, 5+8+2+4, al_map_rgb(0,0,0));
	al_draw_filled_rectangle(24 + 6, 5+8+2, 24 + 4 + 6, 5+8+2+4, al_map_rgb(0,0,0));

	if (data->wins1 >= 1) {
		al_draw_filled_rectangle(24+1, 5+8+2+1, 24 + 4-1, 5+8+2+4-1, al_map_rgb(255,255,255));
	}
	if (data->wins1 >= 2) {
		al_draw_filled_rectangle(24+1+6, 5+8+2+1, 24 + 4-1+6, 5+8+2+4-1, al_map_rgb(255,255,255));
	}

	al_draw_filled_rectangle(320-24-4, 5+8+2, 320-24, 5+8+2+4, al_map_rgb(0,0,0));
	al_draw_filled_rectangle(320-24-4-6, 5+8+2, 320 - 24 - 6, 5+8+2+4, al_map_rgb(0,0,0));

	if (data->wins2 >= 1) {
		al_draw_filled_rectangle(320-24-4+1, 5+8+2+1, 320-24-1, 5+8+2+4-1, al_map_rgb(255,255,255));
	}
	if (data->wins2 >= 2) {
		al_draw_filled_rectangle(320-24-4+1-6, 5+8+2+1, 320-24-1-6, 5+8+2+4-1, al_map_rgb(255,255,255));
	}

	char *timer = malloc(4*sizeof(char));
	snprintf(timer, 4, "%d", data->timer);
	DrawTextWithShadow(data->font, al_map_rgb(255,255,255), 320/2 - 1, 0, ALLEGRO_ALIGN_CENTER, timer);
	free(timer);

	if (data->text) {
		al_draw_filled_rectangle(0, 0, 320, 180, al_map_rgba(0,0,0, 128));
		DrawTextWithShadow(data->font, al_map_rgb(255,255,255), 320/2, 180/2, ALLEGRO_ALIGN_CENTER, data->text);
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


}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->font = al_load_font(GetDataFilePath(game, "fonts/FightingSpirit.ttf"), 16, ALLEGRO_TTF_MONOCHROME);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->bg = al_load_bitmap(GetDataFilePath(game, "bg.png"));
	data->bam1 = al_load_bitmap(GetDataFilePath(game, "bam.png"));
	data->bam2 = al_load_bitmap(GetDataFilePath(game, "bam2.png"));

	data->gand = al_create_bitmap(28,16);

	data->pixelator = al_create_bitmap(320, 180);

	data->wasolele = al_load_bitmap(GetDataFilePath(game, "wasolele.png"));
	data->ukulele = al_load_bitmap(GetDataFilePath(game, "ukulele.png"));
	data->tamburyn = al_load_bitmap(GetDataFilePath(game, "tamburyn.png"));

	for (int i=0; i<24; i++) {
		char *name = malloc(32*sizeof(char));
		snprintf(name, 32, "faces/%d.png", i);
		data->faces[i] = al_load_bitmap(GetDataFilePath(game, name));
		free(name);
	}

	data->player1 = CreateCharacter(game, "player");
	RegisterSpritesheet(game, data->player1, "stand");
	RegisterSpritesheet(game, data->player1, "walk");
	LoadSpritesheets(game, data->player1);
	SelectSpritesheet(game, data->player1, "stand");

	data->player2 = CreateCharacter(game, "player");
	RegisterSpritesheet(game, data->player2, "stand");
	RegisterSpritesheet(game, data->player2, "walk");
	LoadSpritesheets(game, data->player2);
	SelectSpritesheet(game, data->player2, "stand");

	data->gandalf = CreateCharacter(game, "gandalf");
	RegisterSpritesheet(game, data->gandalf, "gandalf");
	LoadSpritesheets(game, data->gandalf);
	SelectSpritesheet(game, data->gandalf, "gandalf");

	int flags = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
	data->collider = al_create_bitmap(320,180);
	data->colplayer = al_create_bitmap(27,71);
	data->colweapon = al_create_bitmap(30,10);
	al_set_target_bitmap(data->colplayer);
	al_clear_to_color(al_map_rgba_f(0.5,0.5,0.5,0.5));
	al_set_target_bitmap(data->colweapon);
	al_clear_to_color(al_map_rgba_f(0.5,0.5,0.5,0.5));
	al_set_target_backbuffer(game->display);
	al_set_new_bitmap_flags(flags);

	for (int i=0; i<8; i++) {
		char *filename = malloc(32*sizeof(char));
		snprintf(filename, 20, "bam%d.wav", i);

		data->bamsample[i] = al_load_sample(GetDataFilePath(game, filename));
		data->bamsound[i] = al_create_sample_instance(data->bamsample[i]);
		al_attach_sample_instance_to_mixer(data->bamsound[i], game->audio.fx);
		al_set_sample_instance_playmode(data->bamsound[i], ALLEGRO_PLAYMODE_ONCE);
	}

	for (int i=0; i<3; i++) {
		char *filename = malloc(32*sizeof(char));
		snprintf(filename, 20, "round%d.wav", i+1);

		data->roundsample[i] = al_load_sample(GetDataFilePath(game, filename));
		data->roundsound[i] = al_create_sample_instance(data->roundsample[i]);
		al_attach_sample_instance_to_mixer(data->roundsound[i], game->audio.voice);
		al_set_sample_instance_playmode(data->roundsound[i], ALLEGRO_PLAYMODE_ONCE);
	}
	data->readysample = al_load_sample(GetDataFilePath(game, "ready.wav"));
	data->readysound = al_create_sample_instance(data->readysample);
	al_attach_sample_instance_to_mixer(data->readysound, game->audio.voice);
	al_set_sample_instance_playmode(data->readysound, ALLEGRO_PLAYMODE_ONCE);

	data->gosample = al_load_sample(GetDataFilePath(game, "go.wav"));
	data->gosound = al_create_sample_instance(data->gosample);
	al_attach_sample_instance_to_mixer(data->gosound, game->audio.voice);
	al_set_sample_instance_playmode(data->gosound, ALLEGRO_PLAYMODE_ONCE);

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
	data->selected1 = game->data->selected1;
	data->selected2 = game->data->selected2;
	data->chosen1 = false;
	data->chosen2 = false;
	data->counter = 0;

	data->colliding1 = false;
	data->colliding2 = false;

data->readiness= 0;
data->round = 0;

  data->posx1 = 10;
	data->posy1 = 70;

	data->posx2 = 280;
	data->posy2 = 80;

	data->rot1 = 0;
	data->rot2 = 3.14;

	data->left1 = false;
	data->left2 = true;

	data->walking1 = false;
	data->walking2 = false;

	data->health1 = 100;
	data->health2 = 100;

	data->bamcount = 0;

	data->text = NULL;

	data->started = false;
	data->timer = 60;
	data->timercount = 0;

	data->wins1 = 0;
	data->wins2 = 0;
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
