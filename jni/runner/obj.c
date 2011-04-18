#include "obj.h"
#include "model.h"
#include "stream.h"
#include "common.h"

#define MAX_VERTICES 2048
#define MAX_INDICES 8192

static vertex_t g_vertices[MAX_VERTICES];
static int g_indices[MAX_INDICES];
static long g_nvertices;
static long g_nindices;

static int add_vertex(const vertex_t* vertex)
{
   if (g_nindices >= MAX_INDICES)
   {
      LOGE("MAX_INDICES limit reached");
      return -1;
   }

   int index = -1;
   int i = 0;
   for (; i < g_nvertices; ++i)
   {
      if (memcmp(&g_vertices[i], vertex, sizeof(vertex_t)) == 0)
      {
         index = i;
         break;
      }
   }

   if (index == -1)
   {
      if (g_nvertices >= MAX_VERTICES)
      {
         LOGE("MAX_VERTICES limit reached");
         return -1;
      }
      index = g_nvertices;
      g_vertices[g_nvertices] = (*vertex);
      ++g_nvertices;
   }

   g_indices[g_nindices] = index;
   ++g_nindices;

   return index;
}

long parse_vertex_component(char** pvert)
{
   long val = 0;
   char* tmp = strsep(pvert, "/");
   if (tmp != NULL)
   {
      val = strtol(tmp, NULL, 10);
   }
   return val;
}

int parse_vertex(char* vert, long* p, long* t, long* n)
{
   long val = parse_vertex_component(&vert);
   if (val == 0)
      return -1;

   (*p) = val - 1;

   val = parse_vertex_component(&vert);
   (*t) = val - 1;

   val = parse_vertex_component(&vert);
   (*n) = val - 1;

   return 0;
}

int parse_face(char** pptr, long* verts, long* pnverts)
{
   long i = 0;
   long maxverts = (*pnverts);

   for (; i < maxverts; ++i)
   {
      char* tmp = strtok_r(NULL, " ", pptr);
      if (tmp == NULL)
      {
         (*pnverts) = i;
         return 0;
      }
      if (parse_vertex(tmp, &verts[i*3 + 0], &verts[i*3 + 1], &verts[i*3 + 2]) != 0)
      {
         LOGE("Unable to parse vertex: '%s'", tmp);
         return -1;
      }
   }
   return 0;
}

long add_face_vertex(long vert[3], vec4f_t* positions, vec4f_t* normals, float* texcoords, long npositions, long nnormals, long ntexcoords)
{
   long p = vert[0];
   long t = vert[1];
   long n = vert[2];

   if (p >= npositions || t >= ntexcoords || n >= nnormals)
   {
      LOGE("Index out of range [%ld %ld] [%ld %ld] [%ld %ld]", p, npositions, t, ntexcoords, n, nnormals);
      return -1;
   }

   vertex_t v;
   memset(&v, 0, sizeof(v));
   v.bone[0] = 0;
   v.weight[0] = 1.0f;

   if (p >= 0)
   {
      v.pos = positions[p];
   }
   if (t >= 0)
   {
      v.tex_coord[0] = texcoords[t*2 + 0];
      v.tex_coord[1] = texcoords[t*2 + 1];
   }
   if (n >= 0)
   {
      v.normal = normals[n];
   }
   add_vertex(&v);
   return 0;
}

int obj_convert(const char* from, const char* to)
{
   LOGI("Loading obj model from %s", from);

   long size = 0;
   char* buf = stream_read_file(from, &size);
   if (buf == NULL)
   {
      LOGE("Unable to read model from %s", from);
      return -1;
   }

   memset(&g_vertices[0], 0, sizeof(g_vertices));
   memset(&g_indices[0], 0, sizeof(g_indices));
   g_nvertices = 0;
   g_nindices = 0;

   vec4f_t positions[MAX_VERTICES];
   vec4f_t normals[MAX_VERTICES];
   float texcoords[MAX_VERTICES * 2];
   long npositions = 0;
   long nnormals = 0;
   long ntexcoords = 0;

   char* ptr = NULL;
   char* str = strtok_r(buf, "\n", &ptr);
   while (str != NULL)
   {
      //LOGI("Processing: %s", str);
      if (str[0] != '#')
      {
         char* ptr2 = NULL;
         char* cmd = strtok_r(str, " ", &ptr2);
         if (cmd != NULL)
         {
            if (strcmp(cmd, "v") == 0)
            {
               // vertex: (x,y,z[,w])
               vec4f_t* p = &positions[npositions];
               p->x = strtof(strtok_r(NULL, " ", &ptr2), NULL);
               p->y = strtof(strtok_r(NULL, " ", &ptr2), NULL);
               p->z = strtof(strtok_r(NULL, " ", &ptr2), NULL);
               ++npositions;
            }
            else if (strcmp(cmd, "vt") == 0)
            {
               // texture coordinate: (u,v[,w])
               texcoords[ntexcoords*2 + 0] = strtof(strtok_r(NULL, " ", &ptr2), NULL);
               texcoords[ntexcoords*2 + 1] = strtof(strtok_r(NULL, " ", &ptr2), NULL);
               ++ntexcoords;
            }
            else if (strcmp(cmd, "vn") == 0)
            {
               // normal (x,y,z)
               vec4f_t* n = &normals[nnormals];
               n->x = strtof(strtok_r(NULL, " ", &ptr2), NULL);
               n->y = strtof(strtok_r(NULL, " ", &ptr2), NULL);
               n->z = strtof(strtok_r(NULL, " ", &ptr2), NULL);
               ++nnormals;
            }
            else if (strcmp(cmd, "f") == 0)
            {
               // face
               long verts[3*4] = {0};
               long nverts = 4;

               parse_face(&ptr2, &verts[0], &nverts);
               if (nverts >= 3)
               {
                  add_face_vertex(&verts[0], positions, normals, texcoords, npositions, nnormals, ntexcoords);
                  add_face_vertex(&verts[6], positions, normals, texcoords, npositions, nnormals, ntexcoords);
                  add_face_vertex(&verts[3], positions, normals, texcoords, npositions, nnormals, ntexcoords);

                  if (nverts == 4)
                  {
                     add_face_vertex(&verts[0], positions, normals, texcoords, npositions, nnormals, ntexcoords);
                     add_face_vertex(&verts[9], positions, normals, texcoords, npositions, nnormals, ntexcoords);
                     add_face_vertex(&verts[6], positions, normals, texcoords, npositions, nnormals, ntexcoords);
                  }
               }
            }
            else if (strcmp(cmd, "o") == 0)
            {
               // [object name]
            }
            else if (strcmp(cmd, "g") == 0)
            {
               // [group name]
            }
            else if (strcmp(cmd, "s") == 0)
            {
               // smooth group [1.., off]
            }
            else if (strcmp(cmd, "mtllib") == 0)
            {
               // material template library [external .mtl filename]
            }
            else if (strcmp(cmd, "usemtl") == 0)
            {
               // use material [material name]
            }
            else
            {
               LOGE("Unknown command: %s", cmd);
            }
         }
      }

      str = strtok_r(NULL, "\n", &ptr);
   }
   free(buf);

   LOGI("Model has %ld positions %ld texture coords %ld normals %ld vertices %ld indices", npositions, ntexcoords, nnormals, g_nvertices, g_nindices);
   long i = 0;
   for (i = 0; i < g_nvertices; ++i)
   {
      LOGI("Vertex #%04ld: %.2f %.2f %.2f [%.2f %.2f %.2f] [%.2f %.2f]", i,
           g_vertices[i].pos.x, g_vertices[i].pos.y, g_vertices[i].pos.z,
           g_vertices[i].normal.x, g_vertices[i].normal.y, g_vertices[i].normal.z,
           g_vertices[i].tex_coord[0], g_vertices[i].tex_coord[1]);
   }

   for (i = 0; i < g_nindices; i += 3)
   {
      LOGI("Tri #%04ld: %d %d %d", i/3, g_indices[i], g_indices[i + 1], g_indices[i + 2]);
   }

   mesh_t mesh;
   memset(&mesh, 0, sizeof(mesh));
   strcpy(mesh.name, "obj");
   strcpy(mesh.shader, "shaders/obj");
   mesh.ntextures = 1;
   mesh.nvertices = g_nvertices;
   mesh.nindices = g_nindices;
   strcpy(mesh.textures[0], "textures/marble.png");
   mesh.vertices = g_vertices;
   mesh.indices = g_indices;

   bone_t bone;
   memset(&bone, 0, sizeof(bone));
   strcpy(bone.name, "root");
   bone.parent = -1;
   bone.pos.x = 0.0f;
   bone.pos.y = 0.0f;
   bone.pos.z = 0.0f;
   bone.pos.w = 0.0f;
   bone.quat.x = 0.0f;
   bone.quat.y = 0.0f;
   bone.quat.z = 0.0f;
   bone.quat.w = 1.0f;

   float transform[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
   anim_t anim;
   memset(&anim, 0, sizeof(anim));
   strcpy(anim.name, "idle");
   anim.nframes = 1;
   anim.transforms = &transform[0];

   model_t mdl;
   memset(&mdl, 0, sizeof(mdl));
   strcpy(mdl.name, to);
   mat4_set_identity(&mdl.transform);
   mdl.nbones = 1;
   mdl.nanims = 1;
   mdl.nmeshes = 1;
   mdl.bones = &bone;
   mdl.anims = &anim;
   mdl.meshes = &mesh;
   model_save(&mdl, to);

   return 0;
}

