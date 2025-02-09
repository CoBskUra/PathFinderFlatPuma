#include "Puma2D.h"

bool Puma2D::AnglesDragFloat(const char* name, float& angle, float mini, float max) {
	float degree = glm::degrees(angle);
	if (ImGui::DragFloat(name, &degree, 1.0f, mini, max - M_ESP, "%.2f")) {
		angle = glm::radians(degree);
		return true;
	}
	return false;
}

void Puma2D::FindPath(Obsticles& obsticles, bool red) {
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


	std::vector<glm::ivec2> result;
	if (red)
		result = corrdsTexture.Path(glm::ivec2{ glm::degrees(tmpStart.alfa[0]), glm::degrees(tmpStart.beta[0]) }, ends);
	else
		result = corrdsTexture.Path(glm::ivec2{ glm::degrees(tmpStart.alfa[1]), glm::degrees(tmpStart.beta[1]) }, ends);






	//corrdsTexture.Draw(result, 0.8f, 1.0f);

	path.clear();
	for (auto& r : result) {
		path.push_back({ glm::radians((float)r.x), glm::radians((float)r.y) });
	}
	// add end
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

glm::vec2 Puma2D::Pos(float a, float b) {
		return (RotationMatrix(a) * TransMatrix_X(settings.L1) * RotationMatrix(b) * TransMatrix_X(settings.L2))[2];
	}

	std::pair<glm::vec2, glm::vec2> Puma2D::TakeMidleAndEndPoint(float a, float b)
	{
		glm::mat3 trans = RotationMatrix(a) * TransMatrix_X(settings.L1);
		glm::vec2 mid = trans[2];
		glm::vec2 end = (trans * RotationMatrix(b) * TransMatrix_X(settings.L2))[2];
		return { mid, end };
	}

	bool Puma2D::ErrorMgWindow(float w, float h) {
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
	

	bool Puma2D::UserInterfers(Obsticles& obsticles) {
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

			if (ImGui::RadioButton("Select red", selectRed)) {
				selectRed = !selectRed;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Select green", !selectRed)) {
				selectRed = !selectRed;
			}

			if (ImGui::Button("Flod-Fill")) {
				FindPath(obsticles, selectRed);
				
				//somethingChanged = true;
			}
			if (!runAnimation && ImGui::Button("Start")) {
				if (step >= path.size() - 1 || step <= 0) {
					step = 0;
					if(!isnan(start.x) && !isnan(destination.x))
						FindPath(obsticles, selectRed);
				}
				runAnimation = !isnan(start.x) && !isnan(destination.x);
				lastTime = 0;
				deltaTime.Reset();
			}
			if (runAnimation && ImGui::Button("Stop")) {
				runAnimation = false; 
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

	bool Puma2D::Inputs(GLFWwindow* window, float width, float height) {
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

	void Puma2D::Draw(bool showOnlyIfReach, Obsticles* o ) {
		if (runAnimation ) {
			step += deltaTime.GetDeltaTime_s() * speed;
			if (step >= path.size()) {
				runAnimation = false;
				step = path.size() - 1;
			}
			else if (step < 0) {
				runAnimation = false;
				step = 0;
			}
			if (path.size() > 0)
			{
				auto first = path[(int)step];
				glm::vec2 value = first;
				if ((int)step < path.size() - 1) {
					auto second = path[(int)step + 1];
					float t = step - (int)step;
					auto diff = first - second;
					if (diff.x < -M_PI)
						first.x += 2 * M_PI;
					else if(diff.x > M_PI)
						first.x -= 2 * M_PI;
					if (diff.y < -M_PI)
						first.y += 2 * M_PI;
					else if (diff.y > M_PI)
						first.y -= 2 * M_PI;
					
					value = first * (1 - t) + second * t;
				}

				auto mid_end = TakeMidleAndEndPoint(value.x, value.y);
				
				auto zero = glm::vec2{ 0, 0 };
				if (value.x > 350) {
					std::cout<<"cos";
				}
				DrawOne(zero, mid_end.first, mid_end.second, {0, 0, 1});
				SetAngles(value.x, value.y);
			}
		}
		else {

			if (!showOnlyIfReach || (o == NULL || !IsIntersect({ 0,0 }, settings.midlePos[0], settings.actualEnd, *o)))
				DrawOne(0, { 1, 0, 0 });
			if (!showOnlyIfReach || (o == NULL || !IsIntersect({ 0,0 }, settings.midlePos[1], settings.actualEnd, *o)))
				DrawOne(1, { 0, 1, 0 });
		}

		glPointSize(10.0f);

		glBegin(GL_POINTS);
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex2f(start.x, start.y);
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex2f(destination.x, destination.y);
		glEnd();
	}

	void Puma2D::DrawOne(int id, glm::vec3 color) {
		auto zero = glm::vec2{ 0,0 };
		DrawOne(zero, settings.midlePos[id], settings.actualEnd, color);
		
	}

	void Puma2D::DrawOne(glm::vec2& start, glm::vec2& mid, glm::vec2& end, glm::vec3 color) {
		glBegin(GL_LINES);
		{
			glColor3f(color.r, color.g, color.b);
			glVertex2f(start.x, start.y);
			glVertex2f(mid.x, mid.y);
			color = color * 0.6f;
			glColor3f(color.r, color.g, color.b);
			glVertex2f(mid.x, mid.y);
			glVertex2f(end.x, end.y);
		}
		glEnd();
	}

	void Puma2D::InversKinematyk(glm::vec2 endPos, Settings& toSetSettings) {
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

	glm::mat3 Puma2D::RotationMatrix(float angle) {
		float s = sinf(angle);
		float c = cosf(angle);
		glm::mat3 result{ 1.f };
		result[0][0] = c;
		result[1][1] = c;

		result[1][0] = -s;
		result[0][1] = s;

		return result;
	}

	glm::mat3 Puma2D::TransMatrix_X(float t) {
		glm::mat3 result{ 1 };
		result[2][2] = 1;
		result[2][0] = t;
		return result;
	}

	void Puma2D::RevaluatedCorrds(Obsticles& obsticles) {
		glm::vec2 zero{ 0, 0 };
		for (int i = 0; i < corrdsTexture.width; i++) {
			float alfa = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(360);
			for (int j = 0; j < corrdsTexture.height; j++) {
				float beta = 2.0f * M_PI * static_cast<float>(j) / static_cast<float>(360);
				auto pair = TakeMidleAndEndPoint(alfa, beta);
				corrdsTexture.Set(i, j,
					!IsIntersect(zero, pair.first, pair.second, obsticles)
					? 1 : 0);
			}
		}
		corrdsTexture.RecreatedTexture();
	}

	void Puma2D::SetAngles(float alfa, float beta) {
		auto end = TakeMidleAndEndPoint(alfa, beta).second;
		InversKinematyk(end, settings);
		currentTarget = end;
		settings.reachEnd = true;
	}

	void Puma2D::SetDestination(glm::vec2 newDestination) {
		InversKinematyk(newDestination, settings);
		destination = newDestination;
	}
	bool Puma2D::IsIntersect(glm::vec2 start, glm::vec2 mid, glm::vec2 end, Obsticles& obsticles) {
		
		return 
			obsticles.IsInside(start) ||
			obsticles.IsInside(mid) ||
			obsticles.IsInside(end) ||

			obsticles.IsIntersect(start, mid) ||
			obsticles.IsIntersect(mid, end);
	}