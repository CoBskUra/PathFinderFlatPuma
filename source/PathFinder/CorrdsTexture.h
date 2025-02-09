#pragma once
#define corrdsMax 360

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "Helper/MathOperations.h"
#include "ShaderManadement/Texture.h"
#include "Obsticles.h"
#include <queue>

class CorrdsTexture {
	struct dataMover {
		float value;
		int parentId;
	};
	std::vector< dataMover> valueParent;
	float data[corrdsMax * corrdsMax];
	Texture textureID{ GL_TEXTURE_2D };
public:
	const int width = corrdsMax;
	const int height = corrdsMax;

	CorrdsTexture();


	void RecreatedTexture();

	void inline ImGuiTexture(int w = 360, int h = 360) {
		ImGui::Image((void*)(intptr_t)textureID.ID, ImVec2(w, h));
	}

	void inline Set(int x, int y, float v) {
		data[y * width + x] = v;
	}


	float inline Get(int x, int y) {
		return data[y * width + x];
	}

	int GetId(int x, int y) {
		return y * width + x;
	}

	glm::ivec2 inline GetXY(int id) {
		int y = id / width;
		return { id - y * width, y };
	}

	std::vector<glm::ivec2> Path(glm::ivec2 start, std::vector<glm::ivec2> ends);
	void Draw(std::vector<glm::ivec2> points, float min, float max);
};