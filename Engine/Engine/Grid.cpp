#include "Grid.h"
#include "GL/glew.h"

Grid::Grid()
{
}

void Grid::Render() const
{
	glLineWidth(lineWidth);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(lineColor[0], lineColor[1], lineColor[2], lineColor[3]);

	glBegin(GL_LINES);

	float start = -gridSize - fmod(-gridSize, cellSize);
	float end = gridSize - fmod(gridSize, cellSize);

	if (normal.x == 1.0f) {
		// Grid YZ
		for (float i = start; i <= end; i += cellSize)
		{
			glVertex3f(0.0f, i, -gridSize);
			glVertex3f(0.0f, i, gridSize);
			glVertex3f(0.0f, -gridSize, i);
			glVertex3f(0.0f, gridSize, i);
		}
	}
	else if (normal.y == 1.0f) {
		// Grid XZ
		for (float i = start; i <= end; i += cellSize)
		{
			glVertex3f(i, 0.0f, -gridSize);
			glVertex3f(i, 0.0f, gridSize);
			glVertex3f(-gridSize, 0.0f, i);
			glVertex3f(gridSize, 0.0f, i);
		}
	}
	else if (normal.z == 1.0f) {
		// Grid XY
		for (float i = start; i <= end; i += cellSize)
		{
			glVertex3f(i, -gridSize, 0.0f);
			glVertex3f(i, gridSize, 0.0f);
			glVertex3f(-gridSize, i, 0.0f);
			glVertex3f(gridSize, i, 0.0f);
		}
	}

	glEnd();

	glDisable(GL_BLEND);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}