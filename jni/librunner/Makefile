TARGET  := ./librunner.a
SOURCES := matrix.c vector.c quaternion.c frustum.c game.c world.c physworld.c image.c common.c tex2d.c shader.c stream_fs.c bbox.c resman.c material.c
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

