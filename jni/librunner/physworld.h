#pragma once

#include "mathlib.h"

typedef struct physworld_t physworld_t;
typedef struct rigidbody_t rigidbody_t;
typedef struct collshape_t collshape_t;

int physworld_create(physworld_t** pw);
void physworld_free(physworld_t* w);
void physworld_update(physworld_t* w, float dt);
void physworld_add_rigidbody(physworld_t* w, rigidbody_t* b);
void physworld_remove_rigidbody(physworld_t* w, rigidbody_t* b);

int collshape_create_sphere(collshape_t** ps, float radius);
int collshape_create_box(collshape_t** ps, float width, float height, float depth);
void collshape_free(collshape_t* s);

int rigidbody_create(rigidbody_t** pb, float mass, collshape_t* s);
void rigidbody_free(rigidbody_t* b);
void rigidbody_set_transform(rigidbody_t* b, const mat4f_t* transform);
void rigidbody_get_transform(const rigidbody_t* b, mat4f_t* transform);

