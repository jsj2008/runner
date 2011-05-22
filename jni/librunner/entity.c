#include "entity.h"
#include "model.h"
#include "resman.h"
#include "common.h"

extern resman_t* g_resman;

void entity_update(entity_t* e, float dt)
{
   // TODO: update animation
}

void entity_render(const entity_t* e, const struct cam_t* camera)
{
   model_t* model = resman_get_model(g_resman, e->model);
   if (model == NULL)
      return;

   mat4f_t transform;
   rigidbody_get_transform(e->phys, &transform);

   model_render(model, camera, e->frame, &transform);
}

