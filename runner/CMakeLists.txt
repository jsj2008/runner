cmake_minimum_required (VERSION 2.6)

project (runner)

add_subdirectory ("../3rdparty/bullet/bullet-2.79" ${PROJECT_BINARY_DIR}/bullet EXCLUDE_FROM_ALL)
add_subdirectory ("../physics" ${PROJECT_BINARY_DIR}/physics)
add_subdirectory ("../engine" ${PROJECT_BINARY_DIR}/engine)
add_subdirectory ("../tools" ${PROJECT_BINARY_DIR}/tools)
add_subdirectory ("shared" ${PROJECT_BINARY_DIR}/shared)

add_subdirectory ("linux" ${PROJECT_BINARY_DIR}/linux)

