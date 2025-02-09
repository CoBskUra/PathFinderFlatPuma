#include "CorrdsTexture.h"

CorrdsTexture::CorrdsTexture() {
	valueParent.resize(width * height);
	for (int i = 0; i < width * height; i++) {
		data[i] = 0;
		valueParent[i].value = 0;
		valueParent[i].parentId = -1;
	}
}


void CorrdsTexture::RecreatedTexture() {
	textureID.Recreat();
	textureID.Bind();

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, data);
	glGenerateMipmap(GL_TEXTURE_2D);
}


std::vector<glm::ivec2> CorrdsTexture::Path(glm::ivec2 start, std::vector<glm::ivec2> ends) {
	for (int i = 0; i < corrdsMax * corrdsMax; i++) {
		valueParent[i].value = data[i];
		valueParent[i].parentId = -1;
	}

	std::queue<glm::ivec2> Q;
	Q.push(start);
	Set(start.x, start.y, -1);
	int id = -1;
	bool found = false;
	while (!Q.empty()) {
		auto pos = Q.front();
		Q.pop();
		id = GetId(pos.x, pos.y);
		if (id < 0) {
			return {};
		}
		if (valueParent[id].value < 1)
			continue;
		valueParent[id].value = 0.3;
		//

		if (std::any_of(ends.begin(), ends.end(), [=](glm::ivec2& a) { return id == GetId(a.x, a.y); })) {
			found = true;
			break;
		}

		auto kid = glm::ivec2{ MathOperations::Wrap(pos.x + 1, 0, width), pos.y };
		int kidId = GetId(kid.x, kid.y);
		if (valueParent[kidId].parentId == -1) {
			valueParent[kidId].parentId = id;
			Q.push(kid);
			Set(kid.x, kid.y, Get(pos.x, pos.y) - 1);
		}
		kid = glm::ivec2{ MathOperations::Wrap(pos.x - 1, 0, width), pos.y };
		kidId = GetId(kid.x, kid.y);
		if (valueParent[kidId].parentId == -1) {
			valueParent[kidId].parentId = id;
			Q.push(kid);
			Set(kid.x, kid.y, Get(pos.x, pos.y) - 1);
		}
		kid = glm::ivec2{ pos.x, MathOperations::Wrap(pos.y + 1, 0, height) };
		kidId = GetId(kid.x, kid.y);
		if (valueParent[kidId].parentId == -1) {
			valueParent[kidId].parentId = id;
			Q.push(kid);
			Set(kid.x, kid.y, Get(pos.x, pos.y) - 1);
		}
		kid = glm::ivec2{ pos.x, MathOperations::Wrap(pos.y - 1, 0, height) };
		kidId = GetId(kid.x, kid.y);
		if (valueParent[kidId].parentId == -1) {
			valueParent[kidId].parentId = id;
			Q.push(kid);
			Set(kid.x, kid.y, Get(pos.x, pos.y) - 1);
		}

	}

	std::vector<glm::ivec2> result{};

	int startId = GetId(start.x, start.y);
	while (found && id != startId) {
		result.push_back(GetXY(id));
		id = valueParent[id].parentId;
	}
	if (found)
		result.push_back(GetXY(startId));

	std::reverse(result.begin(), result.end());

	float size = result.size();
	for (int i = 0; i < corrdsMax * corrdsMax; i++) {
		if (data[i] < 0) {
			float t = static_cast<float>(-data[i] - 1) / size;
			data[i] = t;
		}
	}

	return result;
}

void CorrdsTexture::Draw(std::vector<glm::ivec2> points, float min, float max) {
	float size = points.size();
	for (int i = 0; i < points.size(); i++) {
		float t = float(i) / size;
		float value = min * (1 - t) + max * t;
		auto p = points[i];
		Set(p.x, p.y, value);
	}
}