import os
import sys
import struct
import bpy
import math
from bpy.props import StringProperty

bl_info = {
    "name": "Runner Scenes Format (.runner)",
    "author": "asqz",
    "version": (0, 1),
    "blender": (2, 5, 8),
    "api": 36302,
    "location": "File > Export > Runner (.runner)",
    "description": "Export Runner Scene Format (.runner)",
    "warning": "",
    "category": "Import-Export"}

header_t = struct.Struct("<8s3L")
texcoord_t = struct.Struct("<2f")
vec3f_t = struct.Struct("<3f")
bbox_t = struct.Struct("<12s12s")
mat4f_t = struct.Struct("<16f")
vertex_t = struct.Struct("<12s12s")
camera_t = struct.Struct("<64sL5f64s64s")
material_t = struct.Struct("<64s64s64s12s12sf")
texture_t = struct.Struct("<64s64s4L")
mesh_t = struct.Struct("<64s7L")
submesh_t = struct.Struct("<64s2L")
uvmap_t = struct.Struct("<64s2L")
lamp_t = struct.Struct("<64s2L4f12s")
node_t = struct.Struct("<64s64sL64s24s84sl")
shape_t = struct.Struct("<L2f12s")
phys_t = struct.Struct("<L8f12s12s24s")
scene_t = struct.Struct("<64s64s12s2L")
world_t = struct.Struct("<64s12L")

def convert_path (path):
   root = os.path.dirname(os.path.join(bpy.data.filepath))
   assets_root = os.path.join(root, bpy.data.scenes[0]['assets_root'])
   filepath = os.path.relpath(os.path.join(root, "./" + path), assets_root)
   print ("FILEPATH: " + filepath)
   return filepath


def pack_uv(uv):
   return texcoord_t.pack(uv[0], uv[1])

def pack_vector(vec):
   return vec3f_t.pack(vec[0], vec[1], vec[2])

def pack_color(color):
   return pack_vector(color)

def pack_bbox(bbox):
   (bbox_min, bbox_max) = bbox
   return bbox_t.pack(pack_vector(bbox_min), pack_vector(bbox_max));

def pack_matrix(mat):
   return mat4f_t.pack(
      mat[0][0],  mat[0][1],  mat[0][2],  mat[0][3],
      mat[1][0],  mat[1][1],  mat[1][2],  mat[1][3],
      mat[2][0],  mat[2][1],  mat[2][2],  mat[2][3],
      mat[3][0],  mat[3][1],  mat[3][2],  mat[3][3])

def pack_vertex(v):
   (point, normal) = v
   return vertex_t.pack(pack_vector(point), pack_vector(normal))

def arr_equal(v1, v2):
   if len(v1) != len(v2):
      return False

   for i in range(0, len(v1)):
      diff = v1[i] - v2[i]
      if math.fabs(diff) > 0.0001:
         return False
   return True

def vertices_equal(v1, v2):
   (p1, n1, uv1) = v1
   (p2, n2, uv2) = v2
   return arr_equal(p1, p2) and arr_equal(n1, n2) and arr_equal(uv1, uv2)

def add_vertex(vertices, uvs, mesh, index, face, uv):
   v = mesh.vertices[index]

   if face.use_smooth:
      normal = v.normal
   else:
      normal = face.normal

   vert = (v.co, normal)

   for i in range(0, len(vertices)):
      if vertices_equal(vert, vertices[i]) and arr_equal(uv, uvs[i]):
         #print("Using vertex: #%d"%i)
         return i

   #print("point: " + str(point) + " norm: "+ str(normal))
   vertices.append(vert)
   uvs.append(uv)
   return len(vertices) - 1

def pack_submesh(material, submesh_indices, offset):
   nindices = len(submesh_indices)
   print("Submesh: %s %d indices"%(material.name, nindices))

   indices = bytes()
   for i in submesh_indices:
      indices += struct.pack('<I', i)

   pindices = offset
   header = submesh_t.pack(material.name.encode('utf-8'), nindices, pindices)
   return (header, indices)

def pack_submeshes(mesh, indices, offset):
   submeshes = []
   for i in range(0, len(mesh.materials)):
      submesh_indices = [index for (material_index, index) in indices if material_index == i]
      if len(submesh_indices) > 0:
         submeshes.append((submesh_indices, mesh.materials[i]))

   print("Submeshes count: %d total indices %d"%(len(submeshes), len(indices)))

   offset += len(submeshes) * submesh_t.size

   header = bytes()
   data = bytes()
   for (submesh_indices, material) in submeshes:
      (h, d) = pack_submesh(material, submesh_indices, offset)
      header += h
      data += d
      offset += len(d)

   return (header + data, len(submeshes))

def pack_vertices(vertices, offset):
   print("%d vertices"%len(vertices))
   data = bytes()
   for vert in vertices:
      data += pack_vertex(vert)

   return (data, len(vertices))

def pack_uvmap(uvmap, offset):
   (name, data) = uvmap
   print("uvmap %s with %d items"%(name, len(data)))

   nuvs = len(data)
   puvs = offset

   uvs = bytes()
   for uv in data:
      print("UV: ", uv[0], uv[1])
      uvs += pack_uv(uv)

   header = uvmap_t.pack(name.encode('utf-8'), nuvs, puvs)

   return (header, uvs)

def pack_uvmaps(uvmaps, offset):
   print("%d uvmaps"%len(uvmaps))
   offset += len(uvmaps) * uvmap_t.size

   header = bytes()
   data = bytes()
   for uvmap in uvmaps:
      (h, d) = pack_uvmap(uvmap, offset)
      header += h
      data += d
      offset += len(d)

   return (header + data, len(uvmaps))

def uvs_equal(uvs1, uvs2):
   for i in range(0, len(uvs1)):
      if not arr_equal(uvs1[i], uvs2[i]):
         return False
   return True

def vert_equal(v1, v2):
   (p1, n1, uvs1) = v1
   (p2, n2, uvs2) = v2
   return arr_equal(p1, p2) and arr_equal(n1, n2) and uvs_equal(uvs1, uvs2)

def build_vert(mesh, face, uvs, i):
   v = mesh.vertices[face.vertices[i]]
   if face.use_smooth:
      normal = v.normal
   else:
      normal = face.normal

   texcoords = []
   for uv in uvs:
      texcoords.append(uv[i])

   return (v.co, normal, texcoords)

def add_vert(verts, mesh, face, uvs, i):
   v = build_vert(mesh, face, uvs, i)
   for index, vert in enumerate(verts):
      if vert_equal(v, vert):
         return index

   index = len(verts)
   verts.append(v)
   return index

def build_vertices_uvmaps(mesh):
   verts = []
   mesh_indices = []
   for index, face in enumerate(mesh.faces):
      uvs = []
      for uvmap in mesh.uv_textures:
         uvs.append(uvmap.data[index].uv)

      i0 = add_vert(verts, mesh, face, uvs, 0);
      i1 = add_vert(verts, mesh, face, uvs, 1);
      for i in range(2, len(face.vertices)):
         i2 = add_vert(verts, mesh, face, uvs, i);

         mesh_indices.append((face.material_index, i0))
         mesh_indices.append((face.material_index, i1))
         mesh_indices.append((face.material_index, i2))

         i1 = i2

   mesh_vertices = [(point, normal) for (point, normal, uvs) in verts]

   mesh_uvmaps = []
   for (index, uvmap) in enumerate(mesh.uv_textures):
      m = [uvs[index] for (point, normal, uvs) in verts]
      mesh_uvmaps.append((uvmap.name, m))

   return (mesh_vertices, mesh_uvmaps, mesh_indices)

def pack_mesh(mesh, offset):
   print("Mesh: " + mesh.name)

   (mesh_vertices, mesh_uvmaps, mesh_indices) = build_vertices_uvmaps(mesh)

   pvertices = offset
   (vertices, nvertices) = pack_vertices(mesh_vertices, pvertices)

   puvmaps = pvertices + len(vertices)
   (uvmaps, nuvmaps) = pack_uvmaps(mesh_uvmaps, puvmaps)

   psubmeshes = puvmaps + len(uvmaps)
   (submeshes, nsubmeshes) = pack_submeshes(mesh, mesh_indices, psubmeshes)

   header = mesh_t.pack(mesh.name.encode('utf-8'), mesh.uv_textures.active_index, nvertices, nuvmaps, nsubmeshes, pvertices, puvmaps, psubmeshes)
   return (header, vertices + uvmaps + submeshes)

def pack_meshes(meshes, offset):
   offset += len(meshes) * mesh_t.size

   header = bytes()
   data = bytes()
   for mesh in meshes:
      (h, d) = pack_mesh(mesh, offset)
      header += h
      data += d
      offset += len(d)

   return (header + data, len(meshes))

def get_camera_type(typename):
   if (typename == 'PERSP'):
      return 0
   if (typename == 'ORTHO'):
      return 1
   return -1

def get_shape_type(typename):
   if (typename == 'BOX'):
      return 0
   if (typename == 'SPHERE'):
      return 1
   if (typename == 'CAPSULE'):
      return 2
   if (typename == 'CONE'):
      return 3
   if (typename == 'CYLINDER'):
      return 4
   if (typename == 'CONVEX_HULL'):
      return 5
   if (typename == 'TRIANGLE_MESH'):
      return 6
   return 0

def get_phys_type(typename):
   if (typename == 'RIGID_BODY'):
      return 1
   if (typename == 'STATIC'):
      return 2
   return 0

def get_node_type(typename):
   if (typename == 'MESH'):
      return 0
   if (typename == 'CAMERA'):
      return 1
   if (typename == 'LAMP'):
      return 2
   return -1

def pack_camera(camera):
   print("Camera: " + camera.name)
 
   type = get_camera_type(camera.type)
   fovx = 90.0
   fovy = camera.angle
   aspect = 800/480
   znear = camera.clip_start
   zfar = camera.clip_end
   identity = [[1.0, 0.0, 0.0, 0.0], [0.0, 1.0, 0.0, 0.0], [0.0, 0.0, 1.0, 0.0], [0.0, 0.0, 0.0, 1.0]]

   return camera_t.pack(camera.name.encode('utf-8'), type, fovx, fovy, aspect, znear, zfar, pack_matrix(identity), pack_matrix(identity))

def pack_cameras(cameras, offset):
   data = bytes()
   for camera in cameras:
      data += pack_camera(camera)

   return (data, len(cameras))

def pack_color_scaled(color, scale):
   return pack_color([c * scale for c in color])

def pack_material(material):
   print("Material: " + material.name)
   shader = convert_path(material['shader'])
   texture_slot = material.texture_slots[0]
   texturename = texture_slot.texture.name if texture_slot != None else ""
   return material_t.pack(
         material.name.encode('utf-8'),
         shader.encode('utf-8'),
         texturename.encode('utf-8'),
         pack_color_scaled(material.diffuse_color, material.diffuse_intensity),
         pack_color_scaled(material.specular_color, material.specular_intensity),
         material.specular_hardness)

def pack_materials(materials, offset):
   data = bytes()
   for material in materials:
      data += pack_material(material)

   return (data, len(materials))

GL_NEAREST                 = 0x2600
GL_LINEAR                  = 0x2601
GL_NEAREST_MIPMAP_NEAREST  = 0x2700
GL_LINEAR_MIPMAP_NEAREST   = 0x2701
GL_NEAREST_MIPMAP_LINEAR   = 0x2702
GL_LINEAR_MIPMAP_LINEAR    = 0x2703
GL_REPEAT                  = 0x2901
GL_CLAMP_TO_EDGE           = 0x812F
GL_MIRRORED_REPEAT         = 0x8370

def pack_texture(texture):
   print("Texture: " + texture.name)

   if texture.use_interpolation:
      mag_filter = GL_LINEAR
      if texture.use_mipmap:
         min_filter = GL_LINEAR_MIPMAP_LINEAR
      else:
         min_filter = GL_LINEAR
   else:
      mag_filter = GL_NEAREST
      if texture.use_mipmap:
         min_filter = GL_NEAREST_MIPMAP_NEAREST
      else:
         min_filter = GL_NEAREST

   if texture.extension == "REPEAT":
      if texture.use_mirror_x:
         wrap_s = GL_MIRRORED_REPEAT
      else:
         wrap_s = GL_REPEAT

      if texture.use_mirror_y:
         wrap_t = GL_MIRRORED_REPEAT
      else:
         wrap_t = GL_REPEAT
   else:
      wrap_s = GL_CLAMP_TO_EDGE
      wrap_t = GL_CLAMP_TO_EDGE

   return texture_t.pack(
         texture.name.encode('utf-8'),
         convert_path(texture.image.filepath).encode('utf-8'),
         min_filter, mag_filter,
         wrap_s, wrap_t)

def pack_textures(textures, offset):
   data = bytes()
   for texture in textures:
      data += pack_texture(texture)

   return (data, len(textures))

def get_lamp_type(typename):
   if (typename == 'POINT'):
      return 0
   if (typename == 'SPOT'):
      return 1
   return -1

def get_falloff_type(typename):
   if (typename == 'INVERSE_LINEAR'):
      return 0
   if (typename == 'INVERSE_SQUARE'):
      return 1
   if (typename == 'CONSTANT'):
      return 2
   return -1

def pack_lamp(lamp):
   print("Lamp: " + lamp.name)

   type = get_lamp_type(lamp.type)
   falloff_type = get_falloff_type(lamp.falloff_type)

   if issubclass(lamp.__class__, bpy.types.SpotLamp):
      spot_size = lamp.spot_size
      spot_blend = lamp.spot_blend
   else:
      spot_size = 0.0
      spot_blend = 0.0

   return lamp_t.pack(
         lamp.name.encode('utf-8'),
         type, falloff_type,
         lamp.energy, lamp.distance,
         spot_size, spot_blend,
         pack_color(lamp.color))

def pack_lamps(lamps, offset):
   data = bytes()
   for lamp in lamps:
      data += pack_lamp(lamp)

   return (data, len(lamps))

def get_bbox(node):
   bbox_min = [999999, 999999, 999999]
   bbox_max = [-999999, -999999, -999999]
   for p in node.bound_box:
      for i in range(0, 3):
         if bbox_min[i] > p[i]:
            bbox_min[i] = p[i]
         if bbox_max[i] < p[i]:
            bbox_max[i] = p[i]


   print("Bbox: " + str(bbox_min) + ": "+ str(bbox_max))
   return (bbox_min, bbox_max)

def pack_scene_node(node, parent_index):
   bbox = get_bbox(node)
   return node_t.pack(
         node.name.encode('utf-8'),
         node.data.name.encode('utf-8'),
         get_node_type(node.type),
         pack_matrix(node.matrix_local),
         pack_bbox(bbox),
         pack_phys(node.game, bbox),
         parent_index)

def build_nodes_list(root_nodes, offset):
   if len(root_nodes) == 0:
      return []

   nodes = []
   for i in range(0, len(root_nodes)):
      (_, root_node) = root_nodes[i];
      parent_index = offset + i
      for child in root_node.children:
         nodes.append((parent_index, child))

   nodes.extend(build_nodes_list(nodes, offset + len(root_nodes)))
   return nodes

def pack_scene_nodes(nodes, offset):
   sorted_nodes = [(-1, node) for node in nodes if node.type in ['MESH','CAMERA', 'LAMP'] and node.parent == None]
   sorted_nodes.extend(build_nodes_list(sorted_nodes, 0))

   print("Nodes count: %d"%(len(sorted_nodes)))

   data = bytes()
   for (parent_index, node) in sorted_nodes:
      print("Node %s parent %d"%(node.name, parent_index))
      data += pack_scene_node(node, parent_index)

   return (data, len(sorted_nodes))

def pack_scene(scene, offset):
   print("Scene: " + scene.name)

   pnodes = offset
   (data, nnodes) = pack_scene_nodes(scene.objects, pnodes)

   header = scene_t.pack(scene.name.encode('utf-8'), scene.camera.name.encode('utf-8'), pack_vector(scene.gravity), nnodes, pnodes)
   return (header, data)

def pack_scenes(scenes, offset):
   offset += len(scenes) * scene_t.size

   header = bytes()
   data = bytes()
   for scene in scenes:
      (h, d) = pack_scene(scene, offset)
      header += h
      data += d
      offset += len(d)

   return (header + data, len(scenes))

def pack_shape(shape):
   (type, margin, radius, extents) = shape
   return shape_t.pack(type, margin, radius, pack_vector(extents))

def lock_value(lock):
   if lock:
      return 0.0
   else:
      return 1.0

def pack_phys(phys, bbox):
   (bbox_min, bbox_max) = bbox
   extents = [ bbox_max[0] - bbox_min[0],
               bbox_max[1] - bbox_min[1],
               bbox_max[2] - bbox_min[2]]
   linear_factor = [ lock_value(phys.lock_location_x),
                     lock_value(phys.lock_location_y),
                     lock_value(phys.lock_location_z)]
   angular_factor = [lock_value(phys.lock_rotation_x),
                     lock_value(phys.lock_rotation_y),
                     lock_value(phys.lock_rotation_z)]
   return phys_t.pack(
         get_phys_type(phys.physics_type),
         phys.mass, 0.5, 0.1,
         phys.damping, phys.rotation_damping,
         0.8, 1.0,
         phys.form_factor,
         pack_vector(linear_factor),
         pack_vector(angular_factor),
         pack_shape((get_shape_type(phys.collision_bounds_type), phys.collision_margin, phys.radius, extents)))

def pack_world(name, world):
   print("World: " + name)

   pcameras = world_t.size
   (cameras, ncameras) = pack_cameras(world.cameras, pcameras)

   pmaterials = pcameras + len(cameras)
   (materials, nmaterials) = pack_materials(world.materials, pmaterials)

   ptextures = pmaterials + len(materials)
   (textures, ntextures) = pack_textures(world.textures, ptextures)

   pmeshes = ptextures + len(textures)
   (meshes, nmeshes)  = pack_meshes(world.meshes, pmeshes)

   plamps = pmeshes + len(meshes)
   (lamps, nlamps) = pack_lamps(world.lamps, plamps)

   pscenes = plamps + len(lamps)
   (scenes, nscenes) = pack_scenes(world.scenes, pscenes)

   header = world_t.pack(
      name.encode('utf-8'),
      ncameras, nmaterials, ntextures, nmeshes, nlamps, nscenes,
      pcameras, pmaterials, ptextures, pmeshes, plamps, pscenes)

   return header + cameras + materials + textures + meshes + lamps + scenes

def export_runner_world(context, filepath):
   print("EXPORT RUNNER WORLD TO: " + filepath)

   world_name = bpy.path.display_name_from_filepath(filepath)

   data = pack_world(world_name, bpy.data)
   print("World size: %d"%len(data))

   header = header_t.pack("RNNRWRLD".encode('utf-8'), 1, header_t.size, len(data))

   f = open(filepath, 'wb')
   f.write(header)
   f.write(data)
   f.close()

   return {"FINISHED"}

class RunnerExporter(bpy.types.Operator):
   bl_idname = "export.runner"
   bl_label = "Export Runner"
   filename_ext = ".runner"
   filepath = StringProperty(name="File Path", subtype="FILE_PATH")

   def execute(self, context):
      return export_runner_world(context, self.filepath)

def menu_func(self, context):
   default_path = os.path.splitext(bpy.data.filepath)[0] + ".runner"
   print("DEFAULT PATH: " + default_path)
   self.layout.operator(RunnerExporter.bl_idname, text="Runner (.runner)").filepath = default_path

def register():
   bpy.utils.register_module(__name__)
   bpy.types.INFO_MT_file_export.append(menu_func)

def unregister():
   bpy.utils.unregister_module(__name__)
   bpy.types.INFO_MT_file_export.remove(menu_func)

if __name__ == "__main__":
   register()

   argv = sys.argv
   argv = argv[argv.index("--") + 1:]  # get all args after "--"
   out_path = argv[0]

   bpy.ops.export.runner(filepath=out_path)

