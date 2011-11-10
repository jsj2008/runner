#include <stdio.h>
#include <world.h>
#include <game.h>

game_t* game = NULL;

int main()
{
   world_t* world = NULL;
   world_load_from_file(&world, "../../assets/w01d01.runner");
   world_free(world);
   return 0;
}

