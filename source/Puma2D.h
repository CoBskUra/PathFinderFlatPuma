#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "Helper/MathOperations.h"
#include "ShaderManadement/Texture.h"
#include "Obsticles.h"
#include <queue>
#include <Helper/DeltaTime.h>

#define corrdsMax 360

class CorrdsTexture {
	struct dataMover{
		float value;
		int parentId;
	};
	std::vector< dataMover> valueParent;
	float data[corrdsMax * corrdsMax];
	Texture textureID{ GL_TEXTURE_2D };
public:
	const int width = corrdsMax;
	const int height = corrdsMax;

	CorrdsTexture() {
		valueParent.resize(width * height);
		for (int i = 0; i < width * height; i++) {
			data[i] = 0;
			valueParent[i].value = 0;
			valueParent[i].parentId = -1;
		}
	}


	void RecreatedTexture() {
		textureID.Recreat();
		textureID.Bind();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void ImGuiTexture(int w = 360, int h = 360) {
		ImGui::Image((void*)(intptr_t)textureID.ID, ImVec2(w, h));
	}

	void Set(int x, int y, float v) {
		data[y * width + x] = v;
	}


	float Get(int x, int y) {
		return data[y * width + x];
	}

	int GetId(int x, int y) {
		return y * width + x;
	}

	glm::ivec2 GetXY(int id) {
		int y = id / width;
		return { id - y * width, y };
	}

	std::vector<glm::ivec2> Path(glm::ivec2 start, std::vector<glm::ivec2> ends) {
		for (int i = 0; i < corrdsMax * corrdsMax; i++) {
			valueParent[i].value = data[i];
			valueParent[i].parentId = -1;
		}

		std::queue<glm::ivec2> Q;
		Q.push(start);
		int id = -1;
		bool found = false;
		while (!Q.empty()) {
			auto pos = Q.front();
			Q.pop();
			id = GetId(pos.x, pos.y);
			if (valueParent[id].value < 1)
				continue;
			valueParent[id].value = 0.2f;
			
			if (std::any_of(ends.begin(), ends.end(), [=](glm::ivec2& a) { return id == GetId(a.x, a.y); })) {
				found = true;
				break;
			}

			auto kid = glm::ivec2{ MathOperations::Wrap(pos.x + 1, 0, width), pos.y };
			int kidId = GetId(kid.x, kid.y);
			if (valueParent[kidId].parentId == -1) {
				valueParent[kidId].parentId = id;
				Q.push(kid);
			}
			kid = glm::ivec2{ MathOperations::Wrap(pos.x - 1, 0, width), pos.y };
			kidId = GetId(kid.x, kid.y);
			if (valueParent[kidId].parentId == -1) {
				valueParent[kidId].parentId = id;
				Q.push(kid);
			}
			kid = glm::ivec2{ pos.x, MathOperations::Wrap(pos.y + 1, 0, height) };
			kidId = GetId(kid.x, kid.y);
			if (valueParent[kidId].parentId == -1) {
				valueParent[kidId].parentId = id;
				Q.push(kid);
			}
			kid = glm::ivec2{ pos.x, MathOperations::Wrap(pos.y - 1, 0, height) };
			kidId = GetId(kid.x, kid.y);
			if (valueParent[kidId].parentId == -1) {
				valueParent[kidId].parentId = id;
				Q.push(kid);
			}

		}

		std::vector<glm::ivec2> result{};

		int startId = GetId(start.x, start.y);
		while (found && id != startId) {
			result.push_back(GetXY(id));
			id = valueParent[id].parentId;
		}
		if(found)
			result.push_back(GetXY(startId));

		std::reverse(result.begin(), result.end());
		return result;
	}

	void Draw(std::vector<glm::ivec2> points, float value) {
		for (auto& p : points) {
			Set(p.x, p.y, value);
		}
	}
};

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
	bool runAinmation = false;
	float step = 0;
	
	bool AnglesDragFloat(const char* name, float& angle, float mini, float max) {
		float degree = glm::degrees(angle);
		if (ImGui::DragFloat(name, &degree, 1.0f, mini, max - M_ESP, "%.2f")) {
			angle = glm::radians(degree);
			return true;
		}
		return false;
	}

	void FindPath(Obsticles& obsticles){
		RevaluatedCorrds(obsticles);
		Settings tmpStart = settings;
		Settings tmpDestination = settings;

		InversKinematyk(start, tmpStart);
		InversKinematyk(destination, tmpDestination);


		glm::vec2 end_1 = { tmpDestination.alfa[0], tmpDestination.beta[0] };
		glm::vec2 end_2 = { tmpDestination.alfa[1], tmpDestination.beta[1] };
		std::vector<glm::ivec2> ends = {
			glm::ivec2{ glm::degrees(end_1.x), glm::degrees(end_1.y) },
			glm::ivec2{ glm::degrees(end_2.x), glm::degrees(end_2.y) } };

		auto result = corrdsTexture.Path(glm::ivec2{ glm::degrees(tmpStart.alfa[0]), glm::degrees(tmpStart.beta[0]) }, ends);
		auto result_2 = corrdsTexture.Path(glm::ivec2{ glm::degrees(tmpStart.alfa[1]), glm::degrees(tmpStart.beta[1]) }, ends);
		if ((result_2.size() > 0 && result_2.size() < result.size()) ||
			result.size() == 0) {
			result = result_2;
		}




		corrdsTexture.Draw(result, 0.5f);

		path.clear();
		for (auto& r : result) {
			path.push_back({ glm::radians((float)r.x), glm::radians((float)r.y) });
		}
		
		if (path.size() > 0)
		{
			if (MathOperations::PowDistance(end_1, path[path.size() - 1]) > MathOperations::PowDistance(end_2, path[path.size() - 1])) {
				path.push_back(end_2);
			}
			else {
				path.push_back(end_1);
			}
		}

		corrdsTexture.RecreatedTexture();
	}

public:
	glm::vec2 Pos(float a, float b) {
		return (RotationMatrix(a) * TransMatrix_X(settings.L1) * RotationMatrix(b) * TransMatrix_X(settings.L2))[2];
	}

	std::pair<glm::vec2, glm::vec2> TakeMidleAndEndPoint(float a, float b)
	{
		glm::mat3 trans = RotationMatrix(a) * TransMatrix_X(settings.L1);
		glm::vec2 mid = trans[2];
		glm::vec2 end = (trans * RotationMatrix(b) * TransMatrix_X(settings.L2))[2];
		return { mid, end };
	}

	bool ErrorMgWindow(float w, float h) {
		if (!settings.reachEnd) {
			ImGui::SetNextWindowSize({ 400, 50 });
		}
		else {
			ImGui::SetNextWindowSize({ 0, 0 });
		}

		ImGui::Begin("Cant Reach", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		{
			ImGui::SetWindowPos({ w * 2, h * 2 });
			if (!settings.reachEnd) {
				ImGui::SetWindowPos({ 0, 0 });
				ImGui::SetWindowFontScale(1.5f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text("Can not reach point (%.2f; %.2f)", currentTarget.x, currentTarget.y);
				ImGui::PopStyleColor();
			}
		}
		ImGui::End();
		return !settings.reachEnd;
	}
	

	bool UserInterfers(Obsticles& obsticles) {
		bool somethingChanged = false;
		ImGui::BeginGroup(); {
			if(ImGui::DragFloat("First Arm", &settings.L1, 0.1f)) somethingChanged = true;
			if(ImGui::DragFloat("Second Arm", &settings.L2, 0.1f)) somethingChanged = true;
			if (somethingChanged) {
				settings.reachEnd = true;
			}


			if (!(!AnglesDragFloat("Alfa red", settings.alfa[0], 0, 360) &&
				!AnglesDragFloat("Beta red", settings.beta[0], 180, 360)))
			{
				SetAngles(settings.alfa[0], settings.beta[0]);
				somethingChanged = true;
			}
			if (!(!AnglesDragFloat("Alfa green", settings.alfa[1], 0, 360) &&
				!AnglesDragFloat("Beta green", settings.beta[1], 0, 180)))
			{
				SetAngles(settings.alfa[1], settings.beta[1]);
				somethingChanged = true;
			}

			if (ImGui::DragFloat2("Start", &start[0], 0.01f)) {
				somethingChanged = true;
	
			}

			if (ImGui::DragFloat2("Destination", &destination[0], 0.01f)) {
				somethingChanged = true;
			}

			if (ImGui::DragFloat2("Current target", &currentTarget[0], 0.01f)) {
				somethingChanged = true;
			}

			

			if (ImGui::Button("Revaluated puma's corrds")) {
				RevaluatedCorrds(obsticles);
				somethingChanged = true;
			}
			if (ImGui::Button("Flod-Fill")) {
				FindPath(obsticles);
				
				//somethingChanged = true;
			}
			if (!runAinmation && ImGui::Button("Start")) {
				runAinmation = true;
				deltaTime.Reset();
				lastTime = 0;
				if (step >= path.size() - 1) {
					step = 0;
					FindPath(obsticles);
				}
			}
			if (runAinmation && ImGui::Button("Stop")) {
				runAinmation = false; 
			}

			ImGui::SameLine();
			ImGui::DragFloat("speed", &speed, 0.1f);
			corrdsTexture.ImGuiTexture();
		}
		ImGui::EndGroup();

		if (somethingChanged)
			InversKinematyk(currentTarget, settings);

		return somethingChanged;
	}

	bool Inputs(GLFWwindow* window, float width, float height) {
		if (ImGui::GetIO().WantCaptureMouse)
			return false;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				
			glm::vec2 normalizeMousePos = OpenGLHelper::MousePositionOnScreen(window);

			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
				start = normalizeMousePos;
			}
			else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
				destination = normalizeMousePos;
			}
			currentTarget = normalizeMousePos;
			InversKinematyk(normalizeMousePos, settings);
			return true;
		}


		return false;
	}

	void Draw( bool showBotheSolutions = true) {
		if (runAinmation ) {
			step += deltaTime.GetDeltaTime_s() * speed;
			if (step >= path.size()) {
				runAinmation = false;
				step = path.size() - 1;
			}
			if (path.size() > 0)
			{

				auto first = path[(int)step];
				glm::vec2 value = first;
				if ((int)step < path.size() - 1) {
					auto second = path[(int)step + 1];
					float t = step - (int)step;
					value = first * (1 - t) + second * t;
				}
				Settings tmp = settings;
				SetAngles(value.x, value.y);
				if (isnan(settings.actualEnd.x) || isnan(settings.actualEnd.y))
					settings = tmp;
			}
		}


		DrawOne(0, { 1, 0, 0 });
		if(showBotheSolutions)
			DrawOne(1, { 0, 1, 0 });


		glPointSize(10.0f);

		glBegin(GL_POINTS);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex2f(start.x, start.y);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex2f(destination.x, destination.y);
		glEnd();
	}

	void DrawOne(int id, glm::vec3 color) {
		glBegin(GL_LINES);
		{
			glColor3f(color.r, color.g, color.b);
			glVertex2f(0.0f, 0.0f);
			glVertex2f(settings.midlePos[id].x, settings.midlePos[id].y);
			color = color * 0.6f;
			glColor3f(color.r, color.g, color.b);
			glVertex2f(settings.midlePos[id].x, settings.midlePos[id].y);
			glVertex2f(settings.actualEnd.x, settings.actualEnd.y);
		}
		glEnd();
	}

	void InversKinematyk(glm::vec2 endPos, Settings& toSetSettings) {
		float powDis = glm::dot(endPos, endPos);
		if (powDis >= powf(toSetSettings.L1 + toSetSettings.L2, 2) || powDis <= powf(toSetSettings.L1 - toSetSettings.L2, 2)) {

			const float dis = sqrt(powDis);

			toSetSettings.alfa[0] = toSetSettings.alfa[1] = atan2(endPos.y, endPos.x);
			toSetSettings.beta[0] = toSetSettings.beta[1] = 0;
			toSetSettings.midlePos[0] = toSetSettings.midlePos[1] = endPos * toSetSettings.L1 / dis;
			toSetSettings.actualEnd = endPos * (toSetSettings.L1 + toSetSettings.L2) / dis;

			float actualDis = abs(dis - (toSetSettings.L1 + toSetSettings.L2));
			
			if (abs(dis - (toSetSettings.L1 - toSetSettings.L2)) < actualDis) {
				toSetSettings.beta[0] = toSetSettings.beta[1] = M_PI;
				toSetSettings.midlePos[0] = toSetSettings.midlePos[1] = endPos * toSetSettings.L1 / dis;
				toSetSettings.actualEnd = endPos * (toSetSettings.L1 - toSetSettings.L2) / dis;
				actualDis = abs(dis - (toSetSettings.L1 - toSetSettings.L2));
			}
			if (abs(dis - (-toSetSettings.L1 + toSetSettings.L2)) < actualDis) {
				toSetSettings.alfa[0] = toSetSettings.alfa[1] = -toSetSettings.alfa[0];
				toSetSettings.beta[0] = toSetSettings.beta[1] = M_PI;
				toSetSettings.midlePos[0] = toSetSettings.midlePos[1] = -endPos * toSetSettings.L1 / dis;
				toSetSettings.actualEnd = endPos * (-toSetSettings.L1 + toSetSettings.L2) / dis;
			}
			toSetSettings.reachEnd = false;
			return;
		}
		toSetSettings.reachEnd = true;
		toSetSettings.actualEnd = endPos;

		float betaComponent = acosf((toSetSettings.L1 * toSetSettings.L1 + toSetSettings.L2 * toSetSettings.L2 - powDis) / (2.0f * toSetSettings.L1* toSetSettings.L2));
		toSetSettings.beta[0] = M_PI + betaComponent;
		toSetSettings.beta[1] = M_PI - betaComponent;
		if (toSetSettings.beta[0] < 0)
			toSetSettings.beta[0] += 2 * M_PI;
		if (toSetSettings.beta[1] < 0)
			toSetSettings.beta[1] += 2 * M_PI;

		glm::mat3 Tl1_Rb_Tl2 = TransMatrix_X(toSetSettings.L1) * RotationMatrix(toSetSettings.beta[0]) * TransMatrix_X(toSetSettings.L2);
		glm::vec2 g{ Tl1_Rb_Tl2[2][0], Tl1_Rb_Tl2[2][1] };
		toSetSettings.alfa[0] = atan2f(-g.y * endPos.x + g.x * endPos.y, g.x * endPos.x + g.y * endPos.y);

		Tl1_Rb_Tl2 = TransMatrix_X(toSetSettings.L1) * RotationMatrix(toSetSettings.beta[1]) * TransMatrix_X(toSetSettings.L2);
		g = { Tl1_Rb_Tl2[2][0], Tl1_Rb_Tl2[2][1] };
		toSetSettings.alfa[1] = atan2f(-g.y * endPos.x + g.x * endPos.y, g.x * endPos.x + g.y * endPos.y);

		if (toSetSettings.alfa[0] < 0)
			toSetSettings.alfa[0] += 2 * M_PI;
		if (toSetSettings.alfa[1] < 0)
			toSetSettings.alfa[1] += 2 * M_PI;
		
		toSetSettings.midlePos[0] = RotationMatrix(toSetSettings.alfa[0]) * TransMatrix_X(toSetSettings.L1) * glm::vec3(0, 0, 1);
		toSetSettings.midlePos[1] = RotationMatrix(toSetSettings.alfa[1]) * TransMatrix_X(toSetSettings.L1) * glm::vec3(0, 0, 1);
	}

	glm::mat3 RotationMatrix(float angle) {
		float s = sinf(angle);
		float c = cosf(angle);
		glm::mat3 result{ 1.f };
		result[0][0] = c;
		result[1][1] = c;

		result[1][0] = -s;
		result[0][1] = s;

		return result;
	}

	glm::mat3 TransMatrix_X(float t) {
		glm::mat3 result{ 1 };
		result[2][2] = 1;
		result[2][0] = t;
		return result;
	}

	void RevaluatedCorrds(Obsticles& obsticles) {
		glm::vec2 zero{ 0,0 };
		for (int i = 0; i < corrdsTexture.width; i++) {
			float alfa = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(360);
			for (int j = 0; j < corrdsTexture.height; j++) {
				float beta = 2.0f * M_PI * static_cast<float>(j) / static_cast<float>(360);
				auto pair = TakeMidleAndEndPoint(alfa, beta);
				corrdsTexture.Set(i, j, 
					!obsticles.IsInside(zero) &&
					!obsticles.IsInside(pair.first) &&
					!obsticles.IsInside(pair.second) &&

					!obsticles.IsIntersect(zero, pair.first) &&
					!obsticles.IsIntersect(pair.first, pair.second) 
					? 1 : 0);
			}
		}
		corrdsTexture.RecreatedTexture();
	}

	void SetAngles(float alfa, float beta) {
		auto end = TakeMidleAndEndPoint(alfa, beta).second;
		InversKinematyk(end, settings);
		currentTarget = end;
		settings.reachEnd = true;
	}

	void SetDestination(glm::vec2 newDestination) {
		InversKinematyk(newDestination, settings);
		destination = newDestination;
	}


};
