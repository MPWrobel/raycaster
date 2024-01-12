EXEC=raycaster

CC=clang
LIBS=`pkg-config glfw3 --libs` -framework OpenGL
CFLAGS=`pkg-config glfw3 --cflags`

all: build run

build:
	$(CC) raycaster.c -o $(EXEC) -Wall $(LIBS) $(CFLAGS) -Wno-deprecated -g

run:
	./$(EXEC)

clean:
	rm $(EXEC)
