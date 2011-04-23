#include "octree.h"
#include "stream.h"
#include "frustum.h"
#include "common.h"

#define MAX_NODES 256
#define MAX_TRIS 4096
#define MAX_TRIS_PER_NODE 300

static int octree_node_build(octree_node_t* nodes, long parent, long nodeindex, const bbox_t* bbox, const vertex_t* vertices, long nvertices, const int* indices, long nindices, int* tris, long ntris)
{
   octree_node_t* node = &nodes[nodeindex];
   node->parent = parent;
   node->bbox = (*bbox);

   int node_tris[MAX_TRIS];
   long count = 0;
   long l = 0;
   for (l = 0; l < ntris; ++l)
   {
      int tri = tris[l];
      vec4f_t a = vertices[indices[tri + 0]].pos;
      vec4f_t b = vertices[indices[tri + 1]].pos;
      vec4f_t c = vertices[indices[tri + 2]].pos;
      if (bbox_tri_intersection(&node->bbox, &a, &b, &c) != 0)
      {
         node_tris[count++] = l;
      }
   }

   if (count == 0)
   {
      // EMPTY
      return nodeindex;
   }
   else if (count <= MAX_TRIS_PER_NODE)
   {
      // LEAF
      LOGI("\tLeaf %ld parent %ld [%ld tris]", nodeindex, node->parent, count);
      node->ntris = count;
      node->tris = (int*)malloc(node->ntris*sizeof(int));
      memcpy(&node->tris[0], &node_tris[0], node->ntris*sizeof(int));
      for (l = 0; l < 8; ++l)
      {
         node->children[l] = -1;
      }

      return nodeindex + 1;
   }
   else
   {
      // BRANCH
      LOGI("Branch %ld parent %ld [%ld tris]", nodeindex, node->parent, count);

      node->ntris = 0;
      node->tris = NULL;

      vec4f_t size;
      vec4_sub(&size, &node->bbox.max, &node->bbox.min);

      float hx = size.x / 2.0f;
      float hy = size.y / 2.0f;
      float hz = size.z / 2.0f;

      float dirs[] =
      {
         0.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 1.0f,
         0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 1.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 1.0f,
         1.0f, 1.0f, 0.0f,
         1.0f, 1.0f, 1.0f,
      };

      long nextindex = nodeindex + 1;
      for (l = 0; l < 8; ++l)
      {
         bbox_t box =
         {
            .min =
            {
               .x = node->bbox.min.x + hx * dirs[l*3 + 0],
               .y = node->bbox.min.y + hy * dirs[l*3 + 1],
               .z = node->bbox.min.z + hz * dirs[l*3 + 2],
            },
            .max =
            {
               .x = node->bbox.min.x + hx * (dirs[l*3 + 0] + 1.0f),
               .y = node->bbox.min.y + hy * (dirs[l*3 + 1] + 1.0f),
               .z = node->bbox.min.z + hz * (dirs[l*3 + 2] + 1.0f),
            },
         };

         long newindex = octree_node_build(nodes, nodeindex, nextindex, &box, vertices, nvertices, indices, nindices, node_tris, count);
         if (newindex != nextindex)
         {
            node->children[l] = nextindex;
            nextindex = newindex;
         }
         else
         {
            node->children[l] = -1;
         }
      }
      return nextindex;
   }
}

void octree_build(octree_t** po, vertex_t* vertices, long nvertices, int* indices, long nindices)
{
   octree_node_t nodes[128];
   bbox_t box;

   long l = 0;
   bbox_reset(&box);
   for (l = 0; l < nvertices; ++l)
   {
      vertex_t* v = &vertices[l];
      bbox_inflate(&box, &v->pos);
   }

   vec4f_t size;
   vec4_sub(&size, &box.max, &box.min);
   vec4f_t center =
   {
      .x = box.min.x + size.x / 2.0f,
      .y = box.min.y + size.y / 2.0f,
      .z = box.min.z + size.z / 2.0f,
   };

   float width = size.x;
   if (width < size.y)
      width = size.y;
   else if (width < size.z)
      width = size.z;

   bbox_t bbox =
   {
      .min =
      {
         .x = center.x - width / 2.0f,
         .y = center.y - width / 2.0f,
         .z = center.z - width / 2.0f,
      },
      .max =
      {
         .x = center.x + width / 2.0f,
         .y = center.y + width / 2.0f,
         .z = center.z + width / 2.0f,
      },
   };

   LOGI("Cube width: %.2f", width);
   bbox_show(&box);
   bbox_show(&bbox);

   long ntris = nindices/3;
   int* tris = (int*)malloc(ntris * sizeof(int));
   for (l = 0; l < ntris; ++l)
   {
      tris[l] = (int)l;
   }

   memset(&nodes[0], 0, sizeof(nodes));
   long nnodes = octree_node_build(&nodes[0], -1, 0, &bbox, vertices, nvertices, indices, nindices, tris, ntris);

   free(tris);

   long totaltris = 0;
   for (l = 0; l < nnodes; ++l)
   {
      totaltris += nodes[l].ntris;
   }

   LOGI("Total nodes: %ld Total tris: %ld", nnodes, totaltris);

   octree_t* o = (octree_t*)malloc(
                    sizeof(octree_t) +
                    nvertices * sizeof(vertex_t) +
                    nindices * sizeof(int) +
                    nnodes * sizeof(octree_node_t) +
                    totaltris * sizeof(int));

   o->nvertices = nvertices;
   o->nindices = nindices;
   o->nnodes = nnodes;
   o->vertices = (vertex_t*)((char*)o + sizeof(octree_t));
   o->indices = (int*)((char*)o->vertices + o->nvertices* sizeof(vertex_t));
   o->nodes = (octree_node_t*)((char*)o->indices + o->nindices * sizeof(int));

   char* nodes_tris = (char*)o->nodes + o->nnodes * sizeof(octree_node_t);
   for (l = 0; l < nnodes; ++l)
   {
      octree_node_t* node = &nodes[l];
      if (node->ntris > 0)
      {
         memcpy(nodes_tris, &node->tris[0], node->ntris * sizeof(int));
         free(node->tris);

         node->tris = (int*)nodes_tris;
         nodes_tris += node->ntris * sizeof(int);
      }
   }

   memcpy(&o->vertices[0], &vertices[0], o->nvertices * sizeof(vertex_t));
   memcpy(&o->indices[0], &indices[0], o->nindices * sizeof(int));
   memcpy(&o->nodes[0], &nodes[0], o->nnodes * sizeof(octree_node_t));

   (*po) = o;
}

typedef struct octree_header_t
{
   char magic[8];
   long version;
   long data_offset;
   long data_size;
} octree_header_t;

#define WRITE(n)     { header.data_size += stream_write(f, &n, sizeof(n)); }
#define WRITEA(n, c) { header.data_size += stream_write(f, &n[0], c * sizeof(n[0])); }

int octree_load(octree_t** po, const char* fname)
{
   LOGI("Loading octree from %s", fname);

   if (po == NULL || fname == NULL)
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
   octree_header_t header;
   stream_read(f, &header, sizeof(header));
   if (memcmp(header.magic, "OCTREE__", sizeof(header.magic)) != 0)
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

   octree_t* o = (octree_t*)buf;
   o->vertices = (vertex_t*)(buf + (long)o->vertices);
   o->indices = (int*)(buf + (long)o->indices);
   o->nodes = (octree_node_t*)(buf + (long)o->nodes);

   LOGI("Octree: %ld vertices %ld indices %ld nodes", o->nvertices, o->nindices, o->nnodes);

   long l = 0;
   octree_node_t* node = &o->nodes[0];
   for (l = 0; l < o->nnodes; ++l)
   {
      node->tris = (int*)(buf + (long)node->tris);
      ++node;
   }

   (*po) = o;
   return 0;
}

int octree_save(const octree_t* o, const char* fname)
{
   LOGI("Saving octree to %s", fname);

   stream_t* f = NULL;
   if (stream_open_writer(&f, fname) != 0)
   {
      return -1;
   }

   octree_header_t header =
   {
      .magic = "OCTREE__",
      .version = 1,
      .data_offset = sizeof(octree_header_t),
      .data_size = 0,
   };

   stream_seek(f, header.data_offset, SEEK_SET);

   octree_t octree = (*o);
   octree.vertices = (vertex_t*)(sizeof(octree_t));
   octree.indices = (int*)((char*)octree.vertices + octree.nvertices * sizeof(vertex_t));
   octree.nodes = (octree_node_t*)((char*)octree.indices + octree.nindices * sizeof(int));

   WRITE(octree);
   WRITEA(o->vertices, o->nvertices);
   WRITEA(o->indices, o->nindices);

   long l = 0;

   long offset = (long)((char*)octree.nodes + o->nnodes * sizeof(octree_node_t));
   octree_node_t* node = &o->nodes[0];
   for (l = 0; l < o->nnodes; ++l)
   {
      octree_node_t n = (*node);
      n.tris = (int*)offset;
      offset += n.ntris * sizeof(int);

      WRITE(n);

      ++node;
   }

   node = &o->nodes[0];
   for (l = 0; l < o->nnodes; ++l)
   {
      WRITEA(node->tris, node->ntris);
      ++node;
   }

   // write header
   stream_seek(f, 0, SEEK_SET);
   WRITE(header);

   stream_close(f);
   return 0;
}

void octree_free(octree_t* o)
{
   free((void*)o);
}

void octree_show(const octree_t* o)
{
   if (o == NULL)
   {
      return;
   }

   LOGI("Octree: %ld nodes %ld vertices", o->nnodes, o->nvertices);
   long l = 0;
   for (l = 0; l < o->nnodes; ++l)
   {
      octree_node_t* node = &o->nodes[l];
      if (node->ntris == 0)
      {
         LOGI("\tBranch #%ld: parent #%ld", l, node->parent);
      }
      else
      {
         LOGI("\t\tLeaf #%ld: parent #%ld %ld tris", l, node->parent, node->ntris);
      }
   }

}

void octree_node_draw(const octree_node_t* nodes, long nodeindex, const frustum_t* frustum)
{
   if (nodeindex == -1)
      return;

   const octree_node_t* node = &nodes[nodeindex];
   int res = 2;
   if (frustum != NULL)
   {
      res = frustum_intersect_aabb(frustum, &node->bbox);
   }

   if (res >= 0)
   {

      if (node->ntris == 0)
      {
         LOGI("Branch #%ld parent %ld: res=%d", nodeindex, node->parent, res);
         long l = 0;
         for (l = 0; l < 8; ++l)
         {
            octree_node_draw(nodes, node->children[l], (res == 0) ? frustum : NULL);
         }
      }
      else
      {
         LOGI("\tLeaf #%ld parent %ld %ld tris res=%d", nodeindex, node->parent, node->ntris, res);
      }
   }
}

void octree_draw(const octree_t* o, const cam_t* camera)
{
   frustum_t frustum;
   frustum_set(&frustum, camera);
   frustum_show(&frustum);

   octree_node_draw(o->nodes, 0, &frustum);
}

