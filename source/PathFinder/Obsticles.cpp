#include "Obsticles.h"

bool Obsticles::AddInputs(GLFWwindow* window) {
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
        inProsssesOfAddingRantagle = { {}, {}, {} };

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

bool Obsticles::RemoveInputs(GLFWwindow* window) {
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

void Obsticles::Add(Rantagle r) {
    obsticles.push_back(r);
}

bool Obsticles::RemoveIfInside(glm::vec2 point) {
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

void Obsticles::Draw() {
    for (auto& rantagle : obsticles) {
        rantagle.Draw();
    }
    inProsssesOfAddingRantagle.Draw();
}

bool Obsticles::Inputs(GLFWwindow* window, float width, float height) {
    bool somethingChanged = false;
    if (AddInputs(window)) {
        somethingChanged = true;
    }

    if (RemoveInputs(window)) {
        somethingChanged = true;
    }
    return somethingChanged;
}

bool Obsticles::IsInside(glm::vec2 point) {
    for (auto& o : obsticles) {
        if (o.IsInside(point))
            return true;
    }
    return false;
}

bool Obsticles::IsIntersect(glm::vec2 a, glm::vec2 b) {
    for (auto& o : obsticles) {
        if (o.IsIntersect(a, b))
            return true;
    }
    return false;
}