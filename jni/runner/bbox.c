#include "bbox.h"
#include "common.h"

void bbox_reset(bbox_t* b)
{
   b->min.x = b->min.y = b->min.z = 999999.9f;
   b->max.x = b->max.y = b->max.z = -999999.9f;
}

void bbox_inflate(bbox_t* b, vec4f_t* v)
{
   if (b->min.x > v->x) b->min.x = v->x;
   if (b->min.y > v->y) b->min.y = v->y;
   if (b->min.z > v->z) b->min.z = v->z;
   if (b->max.x < v->x) b->max.x = v->x;
   if (b->max.y < v->y) b->max.y = v->y;
   if (b->max.z < v->z) b->max.z = v->z;
}

void bbox_show(const bbox_t* b)
{
   LOGI("BBox: [%.2f %.2f %.2f] [%.2f %.2f %.2f]", b->min.x, b->min.y, b->min.z, b->max.x, b->max.y, b->max.z);
}

static int point_in_bbox(const bbox_t* box, const vec4f_t* p)
{
   if ((box->min.x <= p->x && box->min.y <= p->y && box->min.z <= p->z) &&
         (box->max.x >= p->x && box->max.y >= p->y && box->max.z >= p->z))
   {
      return 1;
   }
   return 0;
}

int bbox_tri_intersection(const bbox_t* bbox, const vec4f_t* a, const vec4f_t* b, const vec4f_t* c)
{
   if (point_in_bbox(bbox, a) != 0) return 1;
   if (point_in_bbox(bbox, b) != 0) return 1;
   if (point_in_bbox(bbox, c) != 0) return 1;
   return 0;
}

