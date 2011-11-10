cmake_minimum_required (VERSION 2.6)

add_library (physics
   bullet_wrapper.cpp
)

include_directories (.)
include_directories (../engine)
include_directories (../3rdparty/bullet/bullet-2.79/src)

target_link_libraries (physics BulletDynamics BulletCollision LinearMath)

