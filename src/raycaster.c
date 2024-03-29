#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

const int tileSize = 64;

typedef struct {
	unsigned char north : 2;
	unsigned char south : 2;
	unsigned char east  : 2;
	unsigned char west  : 2;
} Tile;

Tile tile = {.north = 3};

typedef struct {
	Vector2 position;
	Vector2 direction;
	Vector2 plane;
	Vector2 size;
	float   speed;
	float   angle;
} Player;

typedef struct {
	Rectangle src, dest;
	Color     tint;
	int       texture;
} Wall;

Texture2D       textures[3];
RenderTexture2D debug;


#define xn {0, 0, 0, 0}
#define xb {1, 1, 1, 1}
#define xw {2, 2, 2, 2}
#define hw {2, 2, 1, 1}
#define vw {1, 1, 2, 2}
Tile map[][16] = {
	{xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb},
	{xb, xn, xn, xb, xn, xn, xn, xn, xn, xn, xn, xn, xw, xb, xn, xb},
	{xb, xn, xn, xb, xn, xn, xn, xn, xn, xn, xn, xn, vw, xb, xn, xb},
	{xb, xn, xn, xb, xn, xn, xn, xn, xn, xn, xn, xn, xn, xn, xn, xb},
	{xb, xn, xn, xn, xn, xn, vw, xn, xn, xn, xn, xb, xb, xb, xb, xb},
	{xb, xn, xn, xn, hw, xn, xn, xn, xn, xn, xn, xb, xn, xn, xn, xb},
	{xb, xn, xn, xn, xn, xn, xn, xn, xn, xn, xn, xb, xn, xn, xn, xb},
	{xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb, xb},
};

Player player = {
	.position = {10.00f, 3.00f},
	.size     = {0.25f,  0.25f},
	.speed    = 0.03f,
	.angle    = PI
};

void
Init()
{
	puts("initializing...");
	textures[1] = LoadTexture("assets/bricks.png");
	textures[2] = LoadTexture("assets/wood.png");
	debug       = LoadRenderTexture(1024, 512);
}

void
Deinit()
{
	puts("deinitializing...");
	for (int i = 1; i < sizeof(textures) / sizeof(*textures); i++) {
		UnloadTexture(textures[i]);
	}
	UnloadRenderTexture(debug);
}

void
Update()
{
	Vector2 verticalMovement   = Vector2Scale(player.direction, player.speed);
	Vector2 horizontalMovement = {-verticalMovement.y, verticalMovement.x};

	if (IsKeyDown(KEY_W)) {
		player.position = Vector2Add(player.position, verticalMovement);
	}

	if (IsKeyDown(KEY_S)) {
		player.position = Vector2Subtract(player.position, verticalMovement);
	}

	if (IsKeyDown(KEY_D)) {
		player.position = Vector2Add(player.position, horizontalMovement);
	}

	if (IsKeyDown(KEY_A)) {
		player.position = Vector2Subtract(player.position, horizontalMovement);
	}

	if (IsKeyDown(KEY_LEFT)) player.angle += player.speed;

	if (IsKeyDown(KEY_RIGHT)) player.angle -= player.speed;

	if (player.angle <= 0) player.angle = 2.0f * PI;
	else if (player.angle >= 2.0f * PI) player.angle = 0.01f;

	player.direction = (Vector2){sinf(player.angle), cosf(player.angle)};
	player.plane     = (Vector2){-cosf(player.angle), sinf(player.angle)};
}

void
DrawMap()
{
	for (int y = 0; y < sizeof(map) / sizeof(*map); y++) {
		for (int x = 0; x < sizeof(*map) / sizeof(**map); x++) {
			Rectangle tile = {x * tileSize, y * tileSize, tileSize, tileSize};
			DrawRectangleLinesEx(tile, 3.0f, DARKGRAY);
			if (map[y][x].north) DrawRectangleRec(tile, WHITE);
		}
	}
}

void
DrawPlayer()
{
	Vector2 player_position = player.position;
	Vector2 player_size     = player.size;
	Vector2 player_origin   = Vector2Scale(player.size, 0.5f);
	Vector2 player_top_left = Vector2Subtract(player.position, player_origin);
	Vector2 plane_position  = Vector2Add(player.position, player.direction);
	Vector2 plane_start     = Vector2Subtract(plane_position, player.plane);
	Vector2 plane_end       = Vector2Add(plane_position, player.plane);

	player_position = Vector2Scale(player_position, tileSize);
	player_size     = Vector2Scale(player_size, tileSize);
	player_top_left = Vector2Scale(player_top_left, tileSize);
	plane_position  = Vector2Scale(plane_position, tileSize);
	plane_start     = Vector2Scale(plane_start, tileSize);
	plane_end       = Vector2Scale(plane_end, tileSize);

	DrawRectangleV(player_top_left, player_size, YELLOW);
	DrawLineV(player_position, plane_position, GREEN);
	DrawLineV(plane_position, plane_start, RED);
	DrawLineV(plane_position, plane_end, RED);
}

void
Draw()
{
	int     rayCount = 1024;
	Vector2 rays[rayCount];
	Wall    walls[rayCount];

	for (int i = 0; i < rayCount; i++) {
		Vector2 skew = Vector2Scale(player.plane, 2.0f * i / rayCount - 1.0f);
		Vector2 ray_direction = Vector2Add(player.direction, skew);

		Vector2 delta = {fabsf(1.0f / ray_direction.x),
		                 fabsf(1.0f / ray_direction.y)};
		Vector2 tile  = {floorf(player.position.x), floorf(player.position.y)};
		Vector2 edge  = Vector2Subtract(player.position, tile);

		Vector2 distance, step;

		if (ray_direction.x > 0) {
			step.x     = 1.0f;
			distance.x = (1 - edge.x) * delta.x;
		} else {
			step.x     = -1.0f;
			distance.x = edge.x * delta.x;
		}

		if (ray_direction.y < 0) {
			step.y     = -1.0f;
			distance.y = edge.y * delta.y;
		} else {
			step.y     = 1.0f;
			distance.y = (1.0f - edge.y) * delta.y;
		}

		bool vertical = true;
		for (;;) {
			if (distance.x < distance.y) {
				distance.x += delta.x;
				tile.x += step.x;
				vertical = true;
			} else {
				distance.y += delta.y;
				tile.y += step.y;
				vertical = false;
			}

			if (map[(int)tile.y][(int)tile.x].north) break;
		}

		distance = Vector2Subtract(distance, delta);

		float   wall_distance = vertical ? distance.x : distance.y;
		Vector2 ray           = Vector2Scale(ray_direction, wall_distance);
		Vector2 contact_point = Vector2Add(player.position, ray);

		rays[i] = Vector2Add(ray, player.position);

		float wall_height = 1.0f / wall_distance;
		float wall_offset = (1.0f - wall_height) / 2.0f;

		wall_height *= 512;
		wall_offset *= 512;

		Vector2 top    = {i, wall_offset};
		Vector2 bottom = {i, wall_offset + wall_height};

		int texture_x = (vertical ? contact_point.y : contact_point.x) * tileSize;
		walls[i].src     = (Rectangle){texture_x, 0.0f, 1.0f, tileSize};
		walls[i].dest    = (Rectangle){top.x, top.y, 1.0f, wall_height};
		if (vertical) {
			walls[i].tint  = WHITE;
			if (contact_point.x <= tile.x) {
				// if (i == 512) puts("west");
				walls[i].texture = map[(int)tile.y][(int)tile.x].west;
			} else {
				// if (i == 512) puts("east");
				walls[i].texture = map[(int)tile.y][(int)tile.x].east;
			}
		} else {
			walls[i].tint  = GRAY;
			if (contact_point.y <= tile.y) {
				// if (i == 512) puts("south");
				walls[i].texture = map[(int)tile.y][(int)tile.x].south;
			} else {
				// if (i == 512) puts("north");
				walls[i].texture = map[(int)tile.y][(int)tile.x].north;
			}
		}
	}

	BeginTextureMode(debug);
	ClearBackground(BLACK);
	DrawMap();
	DrawPlayer();
	for (int i = 0; i < rayCount; i += 16) {
		// if (i != 512) continue;
		DrawLineV(Vector2Scale(player.position, tileSize),
		          Vector2Scale(rays[i], tileSize), ORANGE);
	}
	EndTextureMode();

	BeginDrawing();
	ClearBackground(BLACK);
	DrawRectangle(0, 0, 1024, 256, (Color){120, 120, 120, 255});
	DrawRectangle(0, 256, 1024, 256, (Color){50, 50, 50, 255});
	for (int i = 0; i < rayCount; i++) {
		Texture2D texture = textures[walls[i].texture];
		DrawTexturePro(texture, walls[i].src, walls[i].dest, (Vector2){0}, 0, walls[i].tint);
	}
	DrawTextureEx(debug.texture, (Vector2){0}, 0, 0.5, ColorAlpha(WHITE, 0.5));
	EndDrawing();
}
