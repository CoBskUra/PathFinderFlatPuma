#pragma once
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <Constants.h>
#include <vector>
#include <functional>
#include <glm/ext/matrix_transform.hpp>
//#include "Figures/Bezier/BezierCurve.h"
static class MathOperations {
public:
	static inline bool Inside(float a, float b, const float& t) {
		if (a > b)
			std::swap(a, b);
		return a <= t && b >= t;
	}

	template <typename VecType>
	static float PowDistance(const VecType& vec_1, const VecType& vec_2) {
		VecType diff = vec_1 - vec_2;
		return glm::dot(diff, diff);
	}

	static glm::vec2 Rotation90Degree(const glm::vec2& vec) {
		return { -vec.y, vec.x };
	}

	template<typename T_out, typename T_in>
	static void AppendVector(std::vector<T_out>& vector_out, std::vector<T_in>& vector_in, int start, int end, std::function < T_out(T_in)> transformFun) {
		vector_out.reserve(vector_out.size() - start + end);
		for (int i = start; i < end; i++) {
			vector_out.push_back(transformFun(vector_in[i]));
		}
	}


	static float MinDisFromCenter_Di(int i, const std::vector<glm::vec2>& params, glm::vec2 field, float center, const bool fromRight = true) {
		float dis = M_FLOAT_MAX ;
		for (auto& par : params) {
			float tmpDis = fromRight ? par[i] - center : center - par[i];
			if ( tmpDis < 0)
				continue;
			if (tmpDis < dis)
				dis = tmpDis;
		}
		return dis;
	}

	static glm::mat4 GetRotationXYZ(glm::vec3 angles) {
		return glm::rotate(glm::rotate(glm::rotate(glm::mat4{ 1 },
			angles.x, { 1,0,0 }),
			angles.y, { 0,1,0 }),
			angles.z, { 0,0,1 });
	}

	//static inline glm::vec3  MovedAcrossNormalCloseTo(float t, BezierCurve* curve, float r, glm::vec3 closeToDirection) {
	//	glm::vec3 dereative = glm::normalize(curve->Derivative(t));
	//	glm::vec3 normal = glm::normalize(glm::cross(dereative, glm::cross(dereative, closeToDirection)));
	//	glm::vec3 pos = curve->PositionOnCurve(t);
	//	return pos + normal * r;
	//};

	

	static float MaxDisFromCenter_Di(int i, const std::vector<glm::vec2>& params, glm::vec2 field, float center, const bool fromRight = true) {
		float dis = -1;
		for (auto& par : params) {
			float tmpDis = fromRight ? par[i] - center : center - par[i];
			if (tmpDis < 0)
				continue;
			if (tmpDis > dis)
				dis = tmpDis;
		}
		return dis;
	}

	static int Wrap(int value, int min, int max) {
		value -= min;
		int length = max - min;
		value = value % length;
		value = (length + value) % length;
		return value;
	}

	static float SimpleWrap(const float t, const glm::vec2& field) {
		float length = field.y - field.x;
		if (t > field.y)
			return SimpleWrap(t - length, field);
		else if (t < field.x)
			return SimpleWrap(t + length, field);
		return t;
	}

	static glm::vec3 RotationAlongAxis(glm::vec3 v, float rad, glm::vec3 axis)
	{
		axis = glm::normalize(axis);
		float s = sinf(rad);
		float c = cosf(rad);

		return (1 - c) * glm::dot(v, axis) * axis + c * v + s * glm::cross(axis, v);
	}

	static glm::mat4x4 RotationMatrixAlongAxis(float rad, glm::vec3 axis)
	{
		float s = sinf(rad);
		float c = cosf(rad);
		float t = 1 - c;

		glm::mat4x4 R{ 1.0f };
		R[0][0] = t * axis.x * axis.x + c;
		R[1][1] = t * axis.y * axis.y + c;
		R[2][2] = t * axis.z * axis.z + c;
		R[3][3] = 1.0f;

		R[1][0] = t * axis.x * axis.y - s * axis.z;
		R[2][0] = t * axis.x * axis.z - s * axis.y;
		R[2][1] = t * axis.y * axis.z - s * axis.x;

		R[0][1] = -R[1][0];
		R[0][2] = -R[2][0];
		R[1][2] = -R[2][1];

		return R;
	}

	static void InversThreeDiagonalMatrix(const int n, glm::vec3* x,
		float* a, float* b,
		float* c, glm::vec3* scratch) {
		/*
		 solves Ax = d, where A is a tridiagonal matrix consisting of vectors a, b, c
		 X = number of equations
		 x[] = initially contains the input v, and returns x. indexed from [0, ..., X - 1]
		 a[] = subdiagonal, indexed from [1, ..., X - 1]
		 b[] = main diagonal, indexed from [0, ..., X - 1]
		 c[] = superdiagonal, indexed from [0, ..., X - 2]
		 scratch[] = scratch space of length X, provided by caller, allowing a, b, c to be const
		 not performed in this example: manual expensive common subexpression elimination
		 */
		scratch[0] = glm::vec3{ 1, 1, 1 } *(c[0] / b[0]);
		x[0] = x[0] / b[0];

		/* loop from 1 to X - 1 inclusive */
		for (int ix = 1; ix < n; ix++) {
			if (ix < n - 1) {
				scratch[ix] = c[ix] / (b[ix] - a[ix] * scratch[ix - 1]);
			}
			x[ix] = (x[ix] - a[ix] * x[ix - 1]) / (b[ix] - a[ix] * scratch[ix - 1]);
		}

		/* loop from X - 2 to 0 inclusive */
		for (int ix = n - 2; ix >= 0; ix--)
			x[ix] -= scratch[ix] * x[ix + 1];
	}

	static std::vector<glm::vec3> BezierSubDivide(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
	{
		std::vector<glm::vec3> result{ 7 };
		result[0] = p0;
		result[6] = p3;

		p0 = p0 * (1 - t) + t * p1;
		p1 = p1 * (1 - t) + t * p2;
		p2 = p2 * (1 - t) + t * p3;

		result[1] = p0;
		result[5] = p2;

		p0 = p0 * (1 - t) + t * p1;
		p1 = p1 * (1 - t) + t * p2;

		result[2] = p0;
		result[4] = p1;

		p0 = p0 * (1 - t) + t * p1;
		result[3] = p0;

		return result;
	}

	inline static glm::vec3 Bezier3D(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		p0 = p0 * (1 - t) + t * p1;
		p1 = p1 * (1 - t) + t * p2;
		p2 = p2 * (1 - t) + t * p3;

		p0 = p0 * (1 - t) + t * p1;
		p1 = p1 * (1 - t) + t * p2;

		p0 = p0 * (1 - t) + t * p1;

		return p0;
	}

	inline static glm::vec3 Bezier3D_derivative(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
		return Bezier2D(t, 3.0f * (p1 - p0), 3.0f * (p2 - p1),3.0f * (p3 - p2));
	}

	inline static glm::vec3 Bezier2D(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) {
		p0 = p0 * (1 - t) + t * p1;
		p1 = p1 * (1 - t) + t * p2;

		p0 = p0 * (1 - t) + t * p1;

		return p0;
	}

	inline static glm::vec3 BezierND(float t, std::vector<glm::vec3> ps) {
		for (int i = ps.size() - 1; i >= 0; i--) {
			for (int j = 0; j < i; j++) {
				ps[j] = ps[j] * (1 - t) + t * ps[j + 1];
			}
		}
		if (ps.size() == 0)
			return { 0, 0, 0 };

		return ps[0];
	}

	inline static glm::vec3 BezierNDerivative(float t, std::vector<glm::vec3> ps) {
		float degree = ps.size() - 1;
		for (int i = 0; i < ps.size() - 1; i++) {
			ps[i] = degree * (ps[i + 1] - ps[i]);
		}
		ps.erase(std::next(ps.end(), -1));

		return BezierND(t, ps);
	}

	inline static std::vector<glm::vec3> BezierNDerivative_points(std::vector<glm::vec3> ps) {
		float degree = ps.size() - 1;
		for (int i = 0; i < ps.size() - 1; i++) {
			ps[i] = degree * (ps[i + 1] - ps[i]);
		}
		ps.erase(std::next(ps.end(), -1));

		return ps;
	}

	inline static glm::vec2 ParallelVector(glm::vec2 v, bool cloclkWise = true) {
		return cloclkWise ? glm::vec2{v.y, -v.x} : glm::vec2{-v.y, v.x};
	}

	// return intersection point
	static inline glm::vec2 VectorIntersection(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, bool& isOneIntersection, float& s) {

		isOneIntersection = false;
		glm::vec2 v0 = b - a;
		glm::vec2 v1 = d - c;
		glm::vec2 an{ -v0.y, v0.x };

		if (glm::dot(an, v1) == 0) // r�wnoleg�e
		{
			return a + v0 * 0.5f;
		}
		glm::vec2 ac = c - a;
		glm::vec2 ad = d - a;
		float dis_0 = glm::dot(ac, an);
		float dis_1 = glm::dot(ad, an);
		if (dis_0 * dis_1 > 0) // nieprzecinaj� si� 
		{
			return { 0,0 };
		}

		dis_0 = fabsf(dis_0);
		dis_1 = fabsf(dis_1);
		float dis = dis_0 + dis_1;
		glm::vec2 point = (c * dis_1 + d * dis_0) / dis;
		s = -1;
		if (v0.x != 0) {
			s = (point.x - a.x) / v0.x;
		}
		else if (v0.y != 0) {
			s = (point.y - a.y) / v0.y;
		}
		else
		{
			return point;
		}

		if (s <= 1 && s >= 0)
			isOneIntersection = true;

		return point;
	}
	
};