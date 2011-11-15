cmake_minimum_required (VERSION 2.6)

set (LEVELS
   w01d01.blend
)

set (PROCESSED_LEVELS)
foreach (_file ${LEVELS})
   string (REPLACE ".blend" ".runner" PROCESSED_LEVEL_NAME ${_file})
   set (PROCESSED_LEVEL "${CMAKE_CURRENT_BINARY_DIR}/${PROCESSED_LEVEL_NAME}")
   add_custom_command (
      OUTPUT ${PROCESSED_LEVEL}
      COMMAND blender "${CMAKE_CURRENT_SOURCE_DIR}/${_file}" --background --python "${PROJECT_SOURCE_DIR}/../tools/io_export_runner.py" -- "${PROCESSED_LEVEL}" ${MAKE_SILENT}
      DEPENDS ${_file}
   )
   list (APPEND PROCESSED_LEVELS ${PROCESSED_LEVEL})
endforeach ()

add_custom_target (levels
   DEPENDS ${PROCESSED_LEVELS}
)

install (FILES ${PROCESSED_LEVELS} DESTINATION data/levels)

