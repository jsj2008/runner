cmake_minimum_required (VERSION 2.6)

find_package (OpenGL REQUIRED)

add_library (engine
   world.c
   vector.c
   stream_fs.c
   tex2d.c
   resman.c
   shader.c
   matrix.c
   quaternion.c
   game.c
   image.c
   material.c
   gui.c
   frustum.c
   bbox.c
   common.c
)

include_directories (.)
include_directories (../physics)
include_directories (../3rdparty/png)
include_directories (../3rdparty/png/libpng-1.5.6)

target_link_libraries (engine physics png15_static ${OPENGL_gl_LIBRARY})

