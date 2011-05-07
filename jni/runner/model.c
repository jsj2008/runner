#include "model.h"
#include "common.h"
#include "shader.h"
#include "stream.h"
#include "tex2d.h"
#include "resman.h"

typedef struct model_header_t
{
   char magic[8];
   long version;
   long data_offset;
   long data_size;
} model_header_t;

#define WRITE(n)     { header.data_size += stream_write(f, &n, sizeof(n)); }
#define WRITEA(n, c) { header.data_size += stream_write(f, &n[0], c * sizeof(n[0])); }

int model_save(const model_t* model, const char* fname)
{
   LOGI("Saving model %s to %s", model->name, fname);

   stream_t* f = NULL;
   if (stream_open_writer(&f, fname) != 0)
   {
      return -1;
   }

   model_header_t header =
   {
      .magic = "RNNRMDL_",
      .version = 1,
      .data_offset = sizeof(model_header_t),
      .data_size = 0,
   };

   long i = 0;

   // write model info
   stream_seek(f, header.data_offset, SEEK_SET);

   model_t mdl = (*model);
   mdl.bones = (bone_t*)(sizeof(model_t));
   mdl.anims = (anim_t*)((char*)mdl.bones + mdl.nbones * sizeof(bone_t));
   mdl.meshes = (mesh_t*)((char*)mdl.anims + mdl.nanims * sizeof(anim_t));

   WRITE(mdl);
   WRITEA(model->bones, model->nbones);

   long offset = (long)((char*)mdl.meshes + mdl.nmeshes * sizeof(mesh_t));
   anim_t* anim = &model->anims[0];
   for (i = 0; i < mdl.nanims; ++i)
   {
      anim_t a = (*anim);
      a.transforms = (float*)offset;
      offset += anim->nframes * model->nbones * 6 * sizeof(float);

      WRITE(a);

      ++anim;
   }

   mesh_t* mesh = &model->meshes[0];
   for (i = 0; i < mdl.nmeshes; ++i)
   {
      mesh_t m = (*mesh);

      m.vertices = (vertex_t*)offset;
      offset += m.nvertices * sizeof(vertex_t);

      m.indices = (int*)offset;
      offset += m.nindices * sizeof(int);

      WRITE(m);

      ++mesh;
   }

   anim = &model->anims[0];
   for (i = 0; i < model->nanims; ++i)
   {
      WRITEA(anim->transforms, anim->nframes * model->nbones * 6);
      ++anim;
   }

   mesh = &model->meshes[0];
   for (i = 0; i < model->nmeshes; ++i)
   {
      WRITEA(mesh->vertices, mesh->nvertices);
      WRITEA(mesh->indices, mesh->nindices);
      ++mesh;
   }

   // write header
   stream_seek(f, 0, SEEK_SET);
   WRITE(header);

   stream_close(f);
   return 0;
}

int model_load(model_t** m, const char* fname)
{
   LOGI("Loading model from %s", fname);

   if (m == NULL || fname == NULL)
   {
      LOGE("Invalid arguments");
      return -1;
   }

   stream_t* f = NULL;
   if (stream_open_reader(&f, fname) != 0)
   {
      return -1;
   }

   // read header
   model_header_t header;
   stream_read(f, &header, sizeof(model_header_t));
   if (memcmp(header.magic, "RNNRMDL_", sizeof(header.magic)) != 0)
   {
      LOGE("Invalid file signature: %s", header.magic);
      stream_close(f);
      return -1;
   }

   long size = stream_size(f);

   if (header.data_offset + header.data_size > size)
   {
      LOGE("Invalid file size [offset: %ld size: %ld filesize: %ld]", header.data_offset, header.data_size, size);
      stream_close(f);
      return -1;
   }

   LOGI("Header: offset=%ld size=%ld", header.data_offset, header.data_size);

   stream_seek(f, header.data_offset, SEEK_SET);
   char* buf = malloc(header.data_size);
   stream_read(f, buf, header.data_size);
   stream_close(f);

   model_t* mdl = (model_t*)buf;
   mdl->bones = (bone_t*)(buf + (long)mdl->bones);
   mdl->anims = (anim_t*)(buf + (long)mdl->anims);
   mdl->meshes = (mesh_t*)(buf + (long)mdl->meshes);

   LOGI("Model: %s %ld bones %ld anims %ld meshes", mdl->name, mdl->nbones, mdl->nanims, mdl->nmeshes);

   long i = 0;

   bone_t* bone = &mdl->bones[0];
   for (i = 0; i < mdl->nbones; ++i)
   {
      LOGI("Bone: %s parent %d", bone->name, bone->parent);
      ++bone;
   }

   anim_t* anim = &mdl->anims[0];
   for (i = 0; i < mdl->nanims; ++i)
   {
      LOGI("Anim: %s [%ld frames]", anim->name, anim->nframes);
      anim->transforms = (float*)(buf + (long)anim->transforms);
      ++anim;
   }

   mesh_t* mesh = &mdl->meshes[0];
   for (i = 0; i < mdl->nmeshes; ++i)
   {
      LOGI("Mesh: %s [%ld vertices %ld indices]", mesh->name, mesh->nvertices, mesh->nindices);
      mesh->vertices = (vertex_t*)(buf + (long)mesh->vertices);
      mesh->indices = (int*)(buf + (long)mesh->indices);
      ++mesh;
   }

   (*m) = (model_t*)buf;

   return 0;
}

void model_free(const model_t* model)
{
   free((void*)model);
}

#define MAX_BONES 256
#define MAX_VERTICES 4096

extern resman_t* g_resman;

void model_render(const model_t* model, const cam_t* camera, int _frame)
{
   if (model == NULL || camera == NULL)
      return;

   mat4f_t mv;
   mat4f_t mvp;
   mat4_mult(&mv, &camera->view, &model->transform);
   mat4_mult(&mvp, &camera->proj, &mv);

   static int g_debug = 0;

   mat4f_t bone_transforms[MAX_BONES];
   long i = 0;

   long sequence = 2 % model->nanims;

   anim_t* anim = &model->anims[sequence];

   long scale = 5;
   long frame = (_frame/scale) % anim->nframes;
   long next_frame = (frame + 1) % anim->nframes;
   float t = (float)(_frame % scale) / (float)scale;

   float* trans = anim->transforms + 6*model->nbones*frame;
   float* next_trans = anim->transforms + 6*model->nbones*next_frame;

   bone_t* bone = &model->bones[0];
   for (i = 0; i < model->nbones; ++i)
   {
      vec4f_t quat;

      vec4f_t p;
      p.x = trans[0] * (1.0f - t) + next_trans[0] * t;
      p.y = trans[1] * (1.0f - t) + next_trans[1] * t;
      p.z = trans[2] * (1.0f - t) + next_trans[2] * t;
      p.w = 1.0f;

      vec4f_t qc;
      qc.x = trans[3];
      qc.y = trans[4];
      qc.z = trans[5];
      qc.w = sqrt(1.0f - qc.x*qc.x - qc.y*qc.y - qc.z*qc.z);

      vec4f_t qn;
      qn.x = next_trans[3];
      qn.y = next_trans[4];
      qn.z = next_trans[5];
      qn.w = sqrt(1.0f - qn.x*qn.x - qn.y*qn.y - qn.z*qn.z);

      vec4f_t q;
      quat_slerp(&q, &qc, &qn, t);

      quat_mult(&quat, &q, &bone->quat);

      mat4f_t transform;
      mat4_from_quaternion(&transform, &quat);
      transform.m14 = bone->pos.x + p.x;
      transform.m24 = bone->pos.y + p.y;
      transform.m34 = bone->pos.z + p.z;
      transform.m44 = 1.0f;

      if (bone->parent == -1)
      {
         bone_transforms[i] = transform;
      }
      else
      {
         mat4_mult(&bone_transforms[i], &bone_transforms[bone->parent], &transform);
      }

      if (g_debug)
      {
         //LOGI("\nBone #%ld Parent #%d", i, bone->parent);
         LOGI("P0: %8.2f %8.2f %8.2f", transform.m14, transform.m24, transform.m34);
         //LOGI("P1: %8.2f %8.2f %8.2f", bone->pos.x, bone->pos.y, bone->pos.z);
         //LOGI("Q0: %.2f %.2f %.2f %.2f", bone->quat.x, bone->quat.y, bone->quat.z, bone->quat.w);
         //LOGI("Q1: %.2f %.2f %.2f %.2f", q.x, q.y, q.z, q.w);
         LOGI("Q_: %8.2f %8.2f %8.2f %8.2f", quat.x, quat.y, quat.z, quat.w);
         mat4_show(&transform);
         LOGI(" ");
         mat4_show(&bone_transforms[i]);
         LOGI(" ");
      }

      trans += 6;
      next_trans += 6;
      ++bone;
   }

   mesh_t* mesh = &model->meshes[0];
   for (i = 0; i < model->nmeshes; ++i)
   {
      int sampler_id = 0;

      shader_t* shader = resman_get_shader(g_resman, mesh->shader);
      if (shader == NULL)
         continue;

      shader_use(shader);
      shader_set_uniform_matrices(shader, "uMVP", 1, mat4_data(&mvp));
      shader_set_uniform_matrices(shader, "uMV", 1, mat4_data(&mv));
      shader_set_uniform_matrices(shader, "uBoneTransforms", model->nbones, mat4_data(&bone_transforms[0]));
      shader_set_uniform_integers(shader, "uTex", 1, &sampler_id);
      shader_set_attrib_vertices(shader, "aPos", 3, GL_FLOAT, sizeof(vertex_t), &mesh->vertices[0].pos);
      shader_set_attrib_vertices(shader, "aTexCoord", 2, GL_FLOAT, sizeof(vertex_t), &mesh->vertices[0].tex_coord);
      shader_set_attrib_vertices(shader, "aNormal", 3, GL_FLOAT, sizeof(vertex_t), &mesh->vertices[0].normal);
      shader_set_attrib_vertices(shader, "aBoneIndices", 2, GL_FLOAT, sizeof(vertex_t), &mesh->vertices[0].bone);
      shader_set_attrib_vertices(shader, "aBoneWeights", 2, GL_FLOAT, sizeof(vertex_t), &mesh->vertices[0].weight);

      long l = 0;
      for (l = 0; l < mesh->ntextures; ++l)
      {
         tex2d_t* tex = resman_get_texture(g_resman, mesh->textures[l]);
         if (tex == NULL)
            continue;

         tex2d_bind(tex, sampler_id + l);
      }

      glCullFace(GL_FRONT);
      glDrawElements(GL_TRIANGLES, mesh->nindices, GL_UNSIGNED_INT, mesh->indices);

      ++mesh;
   }

   g_debug = 0;
}

