import bpy
import sys
 
argv = sys.argv
argv = argv[argv.index("--") + 1:]  # get all args after "--"
 
out_path = argv[0]
 
bpy.ops.export.runner(filepath=out_path)

