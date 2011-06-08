#include "material.h"
#include "common.h"
#include "resman.h"
#include "tex2d.h"
#include "shader.h"
#include "game.h"

extern struct game_t* game;

void material_bind(const material_t* material, int sampler_id)
{
   shader_t* shader = resman_get_shader(game->resman, material->shader);
   tex2d_t* tex2d = resman_get_texture(game->resman, material->texture);

   shader_use(shader);
   shader_set_uniform_integers(shader, "uTex", 1, &sampler_id);

   tex2d_bind(tex2d, sampler_id);
}

void material_unbind(const material_t* material)
{
   shader_t* shader = resman_get_shader(game->resman, material->shader);
   shader_unuse(shader);
}

