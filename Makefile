EXEC=build/raycaster

CC=clang
CFLAGS  = -std=c99
# CFLAGS  = -Wall -Wextra -pedantic
CFLAGS += -g
# CFLAGS += -fsanitize=address,undefined
CFLAGS += $(shell pkg-config raylib --cflags)

LDFLAGS = $(shell pkg-config raylib --libs-only-L)
LDLIBS  = $(shell pkg-config raylib --libs-only-l)

all: build build/raycaster.dylib build/raycaster

build:
	mkdir build

build/raycaster.dylib: src/raycaster.c
	$(CC) $(CFLAGS) -undefined dynamic_lookup -o $@ $^

build/raycaster: src/main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

run:
	./$(EXEC)

clean:
	rm -rf build
