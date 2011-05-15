#pragma once

struct shader_t;

typedef struct material_t material_t;

int material_load(material_t** m, const char* fname);
void material_free(material_t* m);
void material_show(const material_t* m);
void material_use(const material_t* m, int sampler_id);
struct shader_t* material_get_shader(const material_t* m);

