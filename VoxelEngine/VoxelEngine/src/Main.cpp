#include <cstdlib>

#include "engine/Game.h"
#include "engine/error/GameException.h"
#include "voxel_engine/VoxelEngine.h"

int main(int argc, char* argv[])
{
	try
	{
		Game game;
		game.PushScene<VoxelEngine>(game);

		game.Play();
	}
	catch (const GameException& error)
	{
		error.Log();
		error.DisplayMessageBox();

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}