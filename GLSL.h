//
// Created by 19450 on 2023/3/28.
//

#ifndef OPENGL__GLSL_H_
#define OPENGL__GLSL_H_

#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using glm::vec3;
using glm::vec2;

const float PI = acos(-1);
const float IVPI = 1. / PI;
const float eps = 1e-5;

inline bool eq(float x, float y) {
	return abs(x - y) < eps;
}
inline int sign(float x) {
	return x > 0 ? 1 : -1;
}

inline vec3 operator*(float x, const vec3 &a) {
	return a * x;
}

inline float length(const vec3& a) { return sqrt(a.x * a.x + a.y * a.y + a.z * a.z); }
inline float distance(const vec3& a, const vec3& b) {
	return sqrt(length(a - b));
}
inline vec3 normalize(const vec3& a) {
	return a / length(a);
}
inline float dot(const vec3& a, const vec3& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline vec3 cross(const vec3& a, const vec3& b) {
	return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
inline vec3 abs(const vec3& a) {
	return vec3(abs(a.x), abs(a.y), abs(a.z));
}
inline vec3 sqrt(const vec3& a) {
	return vec3(sqrt(a.x), sqrt(a.y), sqrt(a.z));
}

inline float mix(float x, float y, float t) {
	return x + (y - x) * t;
}
inline vec3 mix(vec3 x, vec3 y, float t) {
	return x + (y - x) * t;
}

#endif //OPENGL__GLSL_H_
