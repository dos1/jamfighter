#define LIBSUPERDERPY_DATA_TYPE struct CommonResources
#include <libsuperderpy.h>

struct CommonResources {
		// Fill in with common data accessible from all gamestates.
		int selected1, selected2, winner;
};

struct CommonResources* CreateGameData(struct Game *game);
void DestroyGameData(struct Game *game, struct CommonResources *data);
