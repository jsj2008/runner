#include <stdio.h>
#include <world.h>
#include <game.h>
#include <image.h>

game_t* game = NULL;

int main()
{
   image_t* i = NULL;
   image_load(&i, "../../assets/textures/grass06.texture");
   image_save(i, "../../assets/textures/grass06__.texture");
   image_free(i);
   return 0;
}

