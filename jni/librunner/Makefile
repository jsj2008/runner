TARGET  := ./librunner.a
SOURCES := gl.c matrix.c vector.c quaternion.c camera.c tex2d.c model.c hlmdl.c shader.c stream_fs.c obj.c bbox.c frustum.c octree.c resman.c material.c physent.c entity.c physworld.c
OBJECTS := $(SOURCES:.c=.o)

CC      = gcc
CFLAGS  = -O2 -Wall -Werror -g -I../libbullet
LDFLAGS = -L. --shared -g -Wl,-Bdynamic
LIBS    = -lm -lGL -lpng -lbullet

all: $(SOURCES) $(TARGET)

clean:
	rm -rf $(OBJECTS) $(TARGET)

$(TARGET): $(OBJECTS)
#	$(CC) $(LDFLAGS) $(CFLAGS) $(LIBS) -o $@ $(OBJECTS)
	ar rcs $(TARGET) $(OBJECTS)

.c.o:
	$(CC) -c $(CFLAGS) $(@:.o=.c)

