EXEC=raycaster

CC=clang
CFLAGS  = -std=c99 -Wall -Wextra -pedantic
CFLAGS += -g
CFLAGS += $(shell pkg-config glfw3 --cflags)

LDFLAGS = $(shell pkg-config glfw3 --libs-only-L)
LDLIBS  = $(shell pkg-config glfw3 --libs-only-l) -framework OpenGL

all: raycaster.dylib raycaster

raycaster.dylib: raycaster.c
	$(CC) $(CFLAGS) -undefined dynamic_lookup -o $@ $^

raycaster: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

run:
	./$(EXEC)

clean:
	rm $(EXEC)
