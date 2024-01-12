#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

int tileSize = 64;

// clang-format off
int mapW = 16, mapH = 8;
char map[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1,
	1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1,
	1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};
// clang-format on

double plane = 0.5;
double playerSize = 0.25;
double posX = 10, posY = 3;
double dirX, dirY;
double planeX, planeY;
double playerAngle = M_PI;
double speed = 0.03;

void update(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_W)) {
		posX += speed * dirX;
		posY += speed * dirY;
	}

	if (glfwGetKey(window, GLFW_KEY_S)) {
		posX -= speed * dirX;
		posY -= speed * dirY;
	}

	if (glfwGetKey(window, GLFW_KEY_D)) {
		posX -= speed * dirY;
		posY += speed * dirX;
	}

	if (glfwGetKey(window, GLFW_KEY_A)) {
		posX += speed * dirY;
		posY -= speed * dirX;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT))
		playerAngle += speed;

	if (glfwGetKey(window, GLFW_KEY_RIGHT))
		playerAngle -= speed;

	if (playerAngle >= 2 * M_PI)
		playerAngle = 0.01;

	if (playerAngle <= 0)
		playerAngle = 2 * M_PI;

	dirX = sin(playerAngle);
	dirY = cos(playerAngle);

	planeX = -cos(playerAngle) * plane;
	planeY = sin(playerAngle) * plane;
}

void drawRect(double x, double y, double w, double h)
{
	x *= tileSize;
	y *= tileSize;
	w *= tileSize;
	h *= tileSize;

	glBegin(GL_LINE_LOOP);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();
}

void fillRect(double x, double y, double w, double h)
{
	x *= tileSize;
	y *= tileSize;
	w *= tileSize;
	h *= tileSize;

	glBegin(GL_QUADS);
	glVertex2i(x, y);
	glVertex2i(x + w, y);
	glVertex2i(x + w, y + h);
	glVertex2i(x, y + h);
	glEnd();
}

void drawLine(double x1, double y1, double x2, double y2)
{
	x1 *= tileSize;
	y1 *= tileSize;
	x2 *= tileSize;
	y2 *= tileSize;

	glBegin(GL_LINES);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glEnd();
}

void drawLineAbs(double x1, double y1, double x2, double y2)
{
	glBegin(GL_LINES);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glEnd();
}

void drawGrid()
{
	glColor3f(0.3f, 0.3f, 0.3f);
	for (int y = 0; y < mapH; y++)
		for (int x = 0; x < mapW; x++)
			drawRect(x, y, 1, 1);
}

void drawWalls()
{
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int y = 0; y < mapH; y++)
		for (int x = 0; x < mapW; x++)
			if (map[y * mapW + x])
				drawRect(x, y, 1, 1);
}

void drawPlayer(double x, double y)
{
	glColor3f(1.0f, 1.0f, 0);
	fillRect(posX, posY, playerSize, playerSize);

	glColor3f(0, 1.0f, 0);
	drawLine(x, y, x + dirX, y + dirY);

	glColor3f(1.0f, 0, 0);
	drawLine(x + dirX, y + dirY, x + dirX - planeX,
			 y + dirY - planeY);
	drawLine(x + dirX, y + dirY, x + dirX + planeX,
			 y + dirY + planeY);
}

void draw(GLFWwindow *window)
{
	glLineWidth(4);

	drawGrid();
	drawWalls();

	double po = playerSize / 2;
	double ox = posX + po, oy = posY + po;
	drawPlayer(ox, oy);

	double rayCount = 512;
	for (int r = 0; r <= rayCount; r++) {
		double rx = 2 * r / rayCount - 1;
		double rayDirX = dirX + planeX * rx;
		double rayDirY = dirY + planeY * rx;

		double deltaV = fabs(1 / rayDirX);
		double deltaH = fabs(1 / rayDirY);
		// double deltaV = sqrt(1 + ((rayDirY*rayDirY) / (rayDirX*rayDirX)));
		// double deltaH = sqrt(1 + ((rayDirX*rayDirX) / (rayDirY*rayDirY)));
		double sideDistV, sideDistH;

		int mapX = ox, mapY = oy;
		int stepX, stepY;

		//	Moving right
		if (rayDirX > 0) {
			stepX = 1;
			sideDistV = (1 - (ox - mapX)) * deltaV;
		}
		//	Moving left
		else {
			stepX = -1;
			sideDistV = (ox - mapX) * deltaV;
		}

		//	Moving up
		if (rayDirY < 0) {
			stepY = -1;
			sideDistH = (oy - mapY) * deltaH;
		}
		//	Moving down
		else {
			stepY = 1;
			sideDistH = (1 - (oy - mapY)) * deltaH;
		}

		bool hit = false;
		bool vertical = true;
		while (!hit) {
			if (sideDistV < sideDistH) {
				sideDistV += deltaV;
				mapX += stepX;
				vertical = true;
			} else {
				sideDistH += deltaH;
				mapY += stepY;
				vertical = false;
			}

			if (map[mapY * mapW + mapX])
				hit = true;
		}

		if (vertical)
			glColor3f(1.0f, 0, 0);
		else
			glColor3f(0, 0, 1.0f);

		// fillRect(mapX, mapY, 1, 1);

		double ray = sqrt(rayDirX * rayDirX + rayDirY * rayDirY);
		rayDirX /= ray;
		rayDirY /= ray;

		double dist;
		if (vertical)
			dist = sideDistV - deltaV;
		else
			dist = sideDistH - deltaH;

		// drawLine(ox, oy, ox+dist*rayDirX, oy+dist*rayDirY);

		glLineWidth(3);
		double wall = 512 / dist;
		double offset = 512 - wall / 2 - 300;
		if (wall > 512)
			wall = 512;
		drawLineAbs(r * 2, offset, r * 2, wall + offset);
		// drawLine(ox, oy, rx, ry);
	}
}

int main(void)
{
	glfwInit();

	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

	GLFWwindow *window = glfwCreateWindow(1024, 512, "", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	glOrtho(0, 1024, 512, 0, 1, -1);

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
		update(window);

		glClear(GL_COLOR_BUFFER_BIT);
		draw(window);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
