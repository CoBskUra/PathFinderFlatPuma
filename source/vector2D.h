#pragma once

#include <vector>

template <typename T> class vector2D {
	const int width;
	const int height;
	std::vector<T> data{};

	vector2D(int w, int h, int size = 100) {
		width = w;
		height = h;
		data.reserve(size);
	}
};