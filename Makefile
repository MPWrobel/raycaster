EXEC=raycaster

CC=clang
CFLAGS  = -std=c99
# CFLAGS  = -Wall -Wextra -pedantic
CFLAGS += -g
# CFLAGS += -fsanitize=address,undefined
CFLAGS += $(shell pkg-config raylib --cflags)

LDFLAGS = $(shell pkg-config raylib --libs-only-L)
LDLIBS  = $(shell pkg-config raylib --libs-only-l)

all: raycaster.dylib raycaster

raycaster.dylib: raycaster.c
	$(CC) $(CFLAGS) -undefined dynamic_lookup -o $@ $^

raycaster: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $^

run:
	./$(EXEC)

clean:
	rm $(EXEC) raycaster.dylib
