#pragma once

#include "glm/glm.hpp"

class Grid
{
public:
	Grid();
	void Render() const;
public:
	glm::vec3 normal = glm::vec3(0, 1, 0);

	float lineColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float cellSize = 1.0f;
	float gridSize = 200.0f;
	float lineWidth = 1.0f;
};