#include "Rantagle.h"

Rantagle::Rantagle(glm::vec2 firstCorner, glm::vec2 SecondCorner, glm::vec3 color) {
	this->firstCorner = firstCorner;
	this->SecondCorner = SecondCorner;
	if (this->firstCorner.x > this->SecondCorner.x)
		std::swap(this->firstCorner.x, this->SecondCorner.x);
	if (this->firstCorner.y > this->SecondCorner.y)
		std::swap(this->firstCorner.x, this->SecondCorner.x);
	this->color = color;
}

bool Rantagle::IsInside(glm::vec2 point) {
	return
		MathOperations::Inside(firstCorner.x, SecondCorner.x, point.x) &&
		MathOperations::Inside(firstCorner.y, SecondCorner.y, point.y);
}

bool Rantagle::IsIntersect(glm::vec2 a, glm::vec2 b) {
	bool result = false;
	float s = 0;
	glm::vec2 corner_0 = firstCorner;
	glm::vec2 corner_1 = { firstCorner.x, SecondCorner.y };
	glm::vec2 corner_2 = SecondCorner;
	glm::vec2 corner_3 = { SecondCorner.x, firstCorner.y };
	MathOperations::VectorIntersection(a, b, corner_0, corner_1, result, s);
	if (result) return result;
	MathOperations::VectorIntersection(a, b, corner_1, corner_2, result, s);
	if (result) return result;
	MathOperations::VectorIntersection(a, b, corner_2, corner_3, result, s);
	if (result) return result;
	MathOperations::VectorIntersection(a, b, corner_3, corner_0, result, s);
	return result;
}

void Rantagle::Draw() {
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_POLYGON);
	{
		glVertex2f(firstCorner.x, firstCorner.y);
		glVertex2f(SecondCorner.x, firstCorner.y);
		glVertex2f(SecondCorner.x, SecondCorner.y);
		glVertex2f(firstCorner.x, SecondCorner.y);
	}
	glEnd();
}