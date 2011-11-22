#
# Try to find Blender executable
# Once done this will define
#
# Blender_FOUND               - TRUE if Blender executable is found
# Blender_BLENDER_EXECUTABLE  - Full path to Blender executable
#

SET (_BLENDER_HINTS
   "[HKEY_LOCAL_MACHINE\\SOFTWARE\\BlenderFoundation;Install_Dir]"
)

SET (_BLENDER_PATHS
   /usr/bin
   /usr/local/bin
   /bin
)

FIND_PROGRAM(Blender_BLENDER_EXECUTABLE
  NAMES blender
  HINTS ${_BLENDER_HINTS}
  PATHS ${_BLENDER_PATHS}
)

include (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS (Blender DEFAULT_MSG Blender_BLENDER_EXECUTABLE )

if (BLENDER_FOUND)
   MESSAGE("Using BLENDER_EXECUTABLE = " ${Blender_BLENDER_EXECUTABLE})
endif (BLENDER_FOUND)

MARK_AS_ADVANCED(
   Blender_BLENDER_EXECUTABLE
)
