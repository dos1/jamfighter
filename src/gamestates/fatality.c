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

		float rot;

		bool chosen1, chosen2;

		int counter;

		ALLEGRO_BITMAP *bg, *pixelator, *bam1, *bam2, *bam;

		ALLEGRO_BITMAP *characters[24], *faces[24];

		ALLEGRO_BITMAP *ukulele, *wasolele, *tamburyn, *kazoo, *keyboard;

		struct Character *player1, *player2;

		ALLEGRO_SAMPLE *bamsample[8];
		ALLEGRO_SAMPLE_INSTANCE *bamsound[8];

		ALLEGRO_SAMPLE *roundsample[3], *himsample, *hersample;
		ALLEGRO_SAMPLE_INSTANCE *roundsound[3], *himsound, *hersound;

		bool started;
		int pos;

};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->blink_counter++;
	if (data->started) {
		data->rot += 0.005;
	}

	if ((data->rot>0.4) && (data->rot<0.415)) {
		al_stop_sample_instance(game->data->music);
		al_play_sample_instance(data->bamsound[2]);
	}
	if (data->rot > 0.7) {
		SwitchCurrentGamestate(game, "select");
	}

}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.

	al_set_target_bitmap(data->pixelator);

	al_draw_bitmap(data->bg, 0, 0, 0);
//	SetCharacterPosition(game, data->gandalf, 211, 71, 0);

	ALLEGRO_BITMAP *weapon, *weaponl;
	weapon = data->tamburyn;
	weaponl = data->tamburyn;

	if (game->data->winner==22) {
		weapon = data->wasolele;
	}	else if (game->data->winner==2) {
		weapon = data->ukulele;
	} else if (game->data->winner==1) {
		weapon = data->keyboard;
	} else if (game->data->winner==15) {
		weapon = data->kazoo;
	}

	if (game->data->winner==22) {
		weapon = data->wasolele;
	}	else if (game->data->winner==2) {
		weapon = data->ukulele;
	} else if (game->data->winner==1) {
		weapon = data->keyboard;
	}else if (game->data->winner==15) {
		weapon = data->kazoo;
	}





	if (game->data->loser==22) {
		weaponl = data->wasolele;
	}	else if (game->data->loser==2) {
		weaponl = data->ukulele;
	}

	if (game->data->loser==22) {
		weaponl = data->wasolele;
	}	else if (game->data->loser==2) {
		weaponl = data->ukulele;
	}


	int posx1 = 110; int posy1 =3;

	al_draw_rotated_bitmap(weaponl, 0, 3, 10 + posx1 + 7, posy1 + 43, -3.14*1.1, 0);
	SetCharacterPosition(game, data->player1, posx1, posy1, 0);
	DrawCharacter(game, data->player1, al_map_rgb(255,255,255), ALLEGRO_FLIP_HORIZONTAL);
	al_draw_bitmap(data->faces[game->data->loser], -5+posx1, posy1 + 7 + data->player1->pos, ALLEGRO_FLIP_HORIZONTAL);


	al_draw_scaled_rotated_bitmap(weapon, 30/2, 10/2, 50, 90, 6, 6, -0.35*3.14 + data->rot, 0 );


	if (!data->started) {
		al_draw_filled_rectangle(0, 0, 320, 180, al_map_rgba(0,0,0, 128));
		if ((data->blink_counter/30)%2==0) {
			DrawTextWithShadow(data->font, al_map_rgb(255,255,255), 320/2, 180/2 - 8, ALLEGRO_ALIGN_CENTER, "Finish him!");
		}
	}

	if (data->rot>=0.4) {
		al_draw_filled_rectangle(0, 0, 320, 180, al_map_rgb(0,0,0));
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

	if ((ev->type==ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) && (ev->joystick.id == al_get_joystick(game->data->winplayer))) {
		data->started = true;
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->font = al_load_font(GetDataFilePath(game, "fonts/FightingSpirit.ttf"), 16, ALLEGRO_TTF_MONOCHROME);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->bg = al_load_bitmap(GetDataFilePath(game, "fatality.png"));

	data->pixelator = al_create_bitmap(320, 180);

	data->wasolele = al_load_bitmap(GetDataFilePath(game, "wasolele.png"));
	data->ukulele = al_load_bitmap(GetDataFilePath(game, "ukulele.png"));
	data->tamburyn = al_load_bitmap(GetDataFilePath(game, "tamburyn.png"));
	data->kazoo = al_load_bitmap(GetDataFilePath(game, "kazoo.png"));
	data->keyboard = al_load_bitmap(GetDataFilePath(game, "keyboard.png"));

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
	data->himsample = al_load_sample(GetDataFilePath(game, "finishhim.wav"));
	data->himsound = al_create_sample_instance(data->himsample);
	al_attach_sample_instance_to_mixer(data->himsound, game->audio.voice);
	al_set_sample_instance_playmode(data->himsound, ALLEGRO_PLAYMODE_ONCE);

	data->hersample = al_load_sample(GetDataFilePath(game, "finishher.wav"));
	data->hersound = al_create_sample_instance(data->hersample);
	al_attach_sample_instance_to_mixer(data->hersound, game->audio.voice);
	al_set_sample_instance_playmode(data->hersound, ALLEGRO_PLAYMODE_ONCE);

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
	data->rot=0;

	data->started = false;

	al_play_sample_instance(data->himsound);
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
