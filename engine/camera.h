#pragma once

#include "mathlib.h"

typedef struct camera_t
{
   char name[64];

   enum
   {
      CAMERA_PERSPECTIVE = 0,
      CAMERA_ORTHO,
   } type;

   float fovx;
   float fovy;
   float aspect;
   float znear;
   float zfar;

   mat4f_t proj;
   mat4f_t view;
} camera_t;

