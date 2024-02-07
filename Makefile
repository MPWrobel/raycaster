EXEC = build/raycaster

CFLAGS  = -std=c99
# CFLAGS  = -Wall -Wextra -pedantic
CFLAGS += -g
# CFLAGS += -fsanitize=address,undefined

ifneq ($(strip $(shell which pkg-config)),)
	CFLAGS += $(shell pkg-config raylib --cflags)
	LDFLAGS = $(shell pkg-config raylib --libs-only-L)
	LDLIBS  = $(shell pkg-config raylib --libs-only-l)
else
	LDLIBS  = -lraylib
endif

ifeq ($(shell uname), Darwin)
	DYNAMIC = -undefined dynamic_lookup
endif
ifeq ($(shell uname), Linux)
	DYNAMIC = -shared -fPIC
endif

all: build build/raycaster.so build/raycaster

build:
	mkdir build

build/raycaster.so: src/raycaster.c
	$(CC) $^ -o $@ $(CFLAGS) $(DYNAMIC)

build/raycaster: src/main.c
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) $(LDLIBS)

run:
	./$(EXEC)

clean:
	rm -rf build
