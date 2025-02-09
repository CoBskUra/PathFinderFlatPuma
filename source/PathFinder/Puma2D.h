#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "Helper/MathOperations.h"
#include "ShaderManadement/Texture.h"
#include "Obsticles.h"
#include <Helper/DeltaTime.h>
#include "CorrdsTexture.h"



class Puma2D {
	struct Settings {
		float alfa[2] = { 0, 0 }, beta[2] = { 0, 0 };
		float L1 = 1, L2 = 1;
		glm::vec2 midlePos[2];
		glm::vec2 actualEnd{};
		bool reachEnd = true;
	};
	Settings settings{};
	CorrdsTexture corrdsTexture{};
	glm::vec2 destination;
	glm::vec2 start;
	glm::vec2 currentTarget;
	float speed = 1.0f;
	std::vector<glm::vec2> path;
	DeltaTime deltaTime{};
	float lastTime = 0;
	bool runAnimation = false;
	float step = 0;
	bool selectRed = true;
	
	bool AnglesDragFloat(const char* name, float& angle, float mini, float max);

	void FindPath(Obsticles& obsticles, bool red);

public:
	glm::vec2 Pos(float a, float b);

	std::pair<glm::vec2, glm::vec2> TakeMidleAndEndPoint(float a, float b);

	bool ErrorMgWindow(float w, float h);
	

	bool UserInterfers(Obsticles& obsticles);

	bool Inputs(GLFWwindow* window, float width, float height);

	void Draw(bool showOnlyIfReach = true, Obsticles* o = NULL);

	void DrawOne(int id, glm::vec3 color);

	void DrawOne(glm::vec2& start, glm::vec2& mid, glm::vec2& end, glm::vec3 color);

	void InversKinematyk(glm::vec2 endPos, Settings& toSetSettings);

	glm::mat3 RotationMatrix(float angle);

	glm::mat3 TransMatrix_X(float t);

	void RevaluatedCorrds(Obsticles& obsticles);

	void SetAngles(float alfa, float beta);

	void SetDestination(glm::vec2 newDestination);
	bool IsIntersect(glm::vec2 start, glm::vec2 mid, glm::vec2 end, Obsticles& obsticles);
	

};
