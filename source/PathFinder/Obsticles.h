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
#include "Rantagle.h"


class Obsticles {
private:
    std::vector<Rantagle> obsticles;
    bool firstClick = true;
    glm::vec2 startMousePos;
    Rantagle inProsssesOfAddingRantagle{ {}, {}, {} };

    bool AddInputs(GLFWwindow* window);
    bool RemoveInputs(GLFWwindow* window);

public:
    void Add(Rantagle r);
    bool RemoveIfInside(glm::vec2 point);
    void Draw();
    bool Inputs(GLFWwindow* window, float width, float height);
    bool IsInside(glm::vec2 point);
    bool IsIntersect(glm::vec2 a, glm::vec2 b);
};