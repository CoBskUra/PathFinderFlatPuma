#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "Helper/MathOperations.h"
#include <imgui.h>
#include <cstdlib> // srand, rand
#include <ctime>   // time(NULL)
#include "Helper/OpenGLHelper.h"

struct Rantagle {
	glm::vec2 firstCorner;
	glm::vec2 SecondCorner;
	glm::vec3 color;

	Rantagle(glm::vec2 firstCorner, glm::vec2 SecondCorner, glm::vec3 color) {
		this->firstCorner = firstCorner;
		this->SecondCorner = SecondCorner;
		if (this->firstCorner.x > this->SecondCorner.x)
			std::swap(this->firstCorner.x, this->SecondCorner.x);
		if (this->firstCorner.y > this->SecondCorner.y)
			std::swap(this->firstCorner.x, this->SecondCorner.x);
		this->color = color;
	}

	bool IsInside(glm::vec2 point) {
		return
			MathOperations::Inside(firstCorner.x, SecondCorner.x, point.x) &&
			MathOperations::Inside(firstCorner.y, SecondCorner.y, point.y);
	}

	bool IsIntersect(glm::vec2 a, glm::vec2 b) {
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

	void Draw() {
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
};

class Obsticles {
	std::vector< Rantagle> obsticles{};
	bool firstClick = true;
	glm::vec2 startMousePos;
	Rantagle  inProsssesOfAddingRantagle{ {},{},{} };

private:
	bool AddInputs(GLFWwindow* window) {
		if (ImGui::GetIO().WantCaptureMouse)
			return false;

		if (firstClick &&
			glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			glm::vec2 normalizeMousePos = OpenGLHelper::MousePositionOnScreen(window);
			startMousePos = normalizeMousePos;

			glm::vec3 randomColor = OpenGLHelper::RandomVec3();

			inProsssesOfAddingRantagle = { startMousePos, normalizeMousePos, randomColor };

			firstClick = false;
			return true;
		}
		else if (!firstClick &&
			glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			firstClick = true;

			glm::vec2 normalizeMousePos = OpenGLHelper::MousePositionOnScreen(window);

			inProsssesOfAddingRantagle.SecondCorner = normalizeMousePos;

			obsticles.push_back(inProsssesOfAddingRantagle);
			inProsssesOfAddingRantagle = { {}, {},{} };

			return true;
		}
		else if (!firstClick &&
			glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			glm::vec2 normalizeMousePos = OpenGLHelper::MousePositionOnScreen(window);

			inProsssesOfAddingRantagle.SecondCorner = normalizeMousePos;


		}

		return false;
	}

	bool RemoveInputs(GLFWwindow* window) {
		if (ImGui::GetIO().WantCaptureMouse)
			return false;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS &&
			glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			glm::vec2 normalizeMousePos = OpenGLHelper::MousePositionOnScreen(window);
			RemoveIfInside(normalizeMousePos);
			
			return true;
		}
		return false;
	}



public:
	void Add(Rantagle r) {
		obsticles.push_back(r);
	}

	bool RemoveIfInside(glm::vec2 point) {
		bool deleted = false;
		for (int i = obsticles.size() - 1; i >= 0; i--) {
			if (obsticles[i].IsInside(point)) {
				obsticles.erase(std::next(obsticles.begin(), i));
				if (!deleted)
					deleted = true;
			}
		}
		return deleted;
	}

	void Draw() {
		for (auto& rantagle: obsticles) {
			rantagle.Draw();
		}
		inProsssesOfAddingRantagle.Draw();
	}

	bool Inputs(GLFWwindow* window, float width, float height) {
		bool somethingChanged = false;
		if (AddInputs(window)) {
			somethingChanged = true;
		}

		if (RemoveInputs(window)) {
			somethingChanged = true;
		}
		return somethingChanged;
	}
	bool IsInside(glm::vec2 point) {
		for (auto& o : obsticles) {
			if (o.IsInside(point))
				return true;
		}
		return false;
	}

	bool IsIntersect(glm::vec2 a, glm::vec2 b) {
		for (auto& o : obsticles) {
			if (o.IsIntersect(a, b))
				return true;
		}
		return false;
	}
};