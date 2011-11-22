cmake_minimum_required (VERSION 2.6)

find_package (Blender REQUIRED)

set (LEVELS
   w01d01.blend
)

set (EXPORT_SCRIPT "${PROJECT_SOURCE_DIR}/../tools/io_export_runner.py")

set (PROCESSED_LEVELS)
foreach (_file ${LEVELS})
   string (REPLACE ".blend" ".runner" PROCESSED_LEVEL_NAME ${_file})
   set (PROCESSED_LEVEL "${CMAKE_CURRENT_BINARY_DIR}/${PROCESSED_LEVEL_NAME}")
   add_custom_command (
      OUTPUT ${PROCESSED_LEVEL}
      COMMAND ${Blender_BLENDER_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/${_file}" --background --python "${EXPORT_SCRIPT}" -- "${PROCESSED_LEVEL}" ${MAKE_SILENT}
      DEPENDS ${_file} ${EXPORT_SCRIPT}
   )
   list (APPEND PROCESSED_LEVELS ${PROCESSED_LEVEL})
endforeach ()

add_custom_target (levels
   DEPENDS ${PROCESSED_LEVELS}
)

install (FILES ${PROCESSED_LEVELS} DESTINATION data/levels)

