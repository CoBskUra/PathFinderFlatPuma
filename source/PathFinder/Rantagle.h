#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "Helper/MathOperations.h"

struct Rantagle {
	glm::vec2 firstCorner;
	glm::vec2 SecondCorner;
	glm::vec3 color;

	Rantagle(glm::vec2 firstCorner, glm::vec2 SecondCorner, glm::vec3 color);
	bool IsInside(glm::vec2 point);
	bool IsIntersect(glm::vec2 a, glm::vec2 b);
	void Draw();
};