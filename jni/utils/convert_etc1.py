#!/usr/bin/env python2

import Image
import struct
import os
import sys
import math

file_t = struct.Struct("<8s3L")
texture_t = struct.Struct("<4L")
mipmap_t = struct.Struct("<4L")

def get_texture_data (fname):
   tmpname = os.tmpnam()

   os.spawnlp(os.P_WAIT, 'etc1tool', 'etc1tool', '--encodeNoHeader', fname, '-o', tmpname)

   f = open(tmpname, 'rb')
   content = f.read()

   data = bytes(content)
   return (data, len(data))

def pack_mipmap (im, offset):
   tmpname = os.tmpnam() + '.png'
   im.save(tmpname)
   (width, height) = im.size

   print("\tmipmap [%dx%d]"%(width, height))

   pdata = offset
   (data, size) = get_texture_data(tmpname)

   header = mipmap_t.pack(size, width, height, pdata)
   return (header, data)

def pack_mipmaps (im, offset):
   (width, height) = im.size
   nmipmaps = int(math.ceil(math.log(min(width, height), 2))) + 1
   tmp = im

   print("texture has %d mipmaps"%nmipmaps)

   offset += nmipmaps * mipmap_t.size

   header = bytes()
   data = bytes()
   for level in range(0, nmipmaps):
      (h, d) = pack_mipmap(tmp, offset)
      header += h
      data += d
      offset += len(d)

      tmp = im.resize((width / 2, height / 2), Image.ANTIALIAS)
      (width, height) = tmp.size

   return (header + data, nmipmaps)

def pack_texture (fname, offset):
   print("processing texture %s"%fname)

   im = Image.open(fname)
   im = im.transpose(Image.FLIP_TOP_BOTTOM)
   bpp = len(im.mode)

   pmipmaps = offset + texture_t.size
   (mipmaps, nmipmaps) = pack_mipmaps(im, pmipmaps)

   header = texture_t.pack(1, bpp, nmipmaps, pmipmaps)

   return (header, mipmaps)

def convert_texture (fname):
   (inname, _) = os.path.splitext(fname)
   outname = inname + '.texture'

   (h, d) = pack_texture(fname, 0)

   data = h + d

   version = 1
   offset = file_t.size
   size = len(data)
   header = file_t.pack("RNNRTXTR".encode("utf-8"), version, offset, size)

   f = open(outname, 'wb')
   f.write(header)
   f.write(data)
   f.close()

if __name__ == '__main__':
   if len(sys.argv) <= 1:
      print("Usage: ./convert_texture.py texture.png")
      exit(1)

   for fname in sys.argv[1:]:
      convert_texture(fname)

