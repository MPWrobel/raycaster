#include <dlfcn.h>
#include <libgen.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void func(void);

static func *Init;
static func *Deinit;
static func *Update;
static func *Draw;

static void *
load(char *path)
{
	static void *mod;

	system("make");

	if (mod) dlclose(mod);
	mod = dlopen(path, RTLD_NOW);

	if (!mod) return mod;

	Init   = (func *)dlsym(mod, "Init");
	Deinit = (func *)dlsym(mod, "Deinit");
	Update = (func *)dlsym(mod, "Update");
	Draw   = (func *)dlsym(mod, "Draw");

	return mod;
}

int
main(int argc, char **argv)
{
	char *bindir  = dirname(argv[0]);
	char *modname = "raycaster.dylib";
	char  modpath[strlen(bindir) + strlen(modname) + 2];
	snprintf(modpath, sizeof(modpath), "%s/%s", bindir, modname);

	InitWindow(1024, 512, "");

	SetTargetFPS(60);
	SetExitKey(KEY_NULL);

	void *mod = load(modpath);
	if (Init) Init();

	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_ESCAPE)) {
			mod = load(modpath);
			if (Deinit) Deinit();
			if (Init) Init();
		}

		if (Update) Update();
		if (Draw) Draw();
	}

	if (Deinit) Deinit();
	if (mod) dlclose(mod);

	CloseWindow();

	return 0;
}
