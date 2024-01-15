#include <dlfcn.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLFW/glfw3.h>

typedef void func(void);

GLFWwindow *window;

static char *modpath;
static func *init;
static func *deinit;
static func *update;
static func *draw;

static void *load(char *path)
{
	static void *mod;

	system("make");

	if (mod) dlclose(mod);
	mod = dlopen(path, RTLD_NOW);

	if (!mod) return mod;

	init   = (func *) dlsym(mod, "init");
	deinit = (func *) dlsym(mod, "deinit");
	update = (func *) dlsym(mod, "update");
	draw   = (func *) dlsym(mod, "draw");

	return mod;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) load(modpath);
}

int main(int argc, char **argv)
{
	glfwInit();

	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

	window = glfwCreateWindow(1024, 512, "", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);

	char *bindir = dirname(argv[0]);
	char *modname = "raycaster.dylib";
	modpath = malloc(strlen(bindir) + strlen(modname) + 1);
	sprintf(modpath, "%s/%s", bindir, modname);

	void *mod = load(modpath);

	if (init) init();

	double previousTime = glfwGetTime();
	int frameCount = 0;
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		frameCount++;

		if (currentTime - previousTime >= 1.0) {
			char title[10];
			sprintf(title, "%d", frameCount);
			glfwSetWindowTitle(window, title);
			frameCount = 0;
			previousTime = currentTime;
		}

		glfwPollEvents();
		if (update) update();

		glClear(GL_COLOR_BUFFER_BIT);
		if (draw) draw();

		glfwSwapBuffers(window);
	}

	if (deinit) deinit();
	
	if (mod) dlclose(mod);
	free(modpath);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
