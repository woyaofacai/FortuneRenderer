#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

using Vector2f = glm::vec2;
using Vector3f = glm::vec3;
using Vector4f = glm::vec4;
using Vector2i = glm::ivec2;
using Vector3i = glm::ivec3;
using Vector4i = glm::ivec4;
using Matrix3x3 = glm::mat3;
using Matrix4x4 = glm::mat4;
using Color = glm::vec3;

const float PI = glm::pi<float>();


inline void DumpVector(const Vector3f& v)
{
	std::cout << "Vector3f(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

// 构造一个平移矩阵：
inline Matrix4x4 MakeTranslation(const Vector3f& t)
{
	return Matrix4x4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		t.x, t.y, t.z, 1.0f
	);
}

// 构造一个旋转矩阵（传入一个欧拉角）
inline Matrix4x4 MakeRotation(const Vector3f& euler)
{
	// 先绕X轴旋转，再绕Y轴旋转，最后绕Z轴旋转：
	float cx = cosf(euler.x);
	float sx = sinf(euler.x);
	float cy = cosf(euler.y);
	float sy = sinf(euler.y);
	float cz = cosf(euler.z);
	float sz = sinf(euler.z);
	Matrix4x4 rx(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cx, sx, 0.0f,
		0.0f, -sx, cx, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Matrix4x4 ry(
		cy, 0.0f, -sy, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sy, 0.0f, cy, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	Matrix4x4 rz(
		cz, sz, 0.0f, 0.0f,
		-sz, cz, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	return rz * ry * rx; // 注意矩阵乘法的顺序
}

inline Matrix4x4 MakeScale(float s)
{
	return Matrix4x4(
		s, 0.0f, 0.0f, 0.0f,
		0.0f, s, 0.0f, 0.0f,
		0.0f, 0.0f, s, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

inline Matrix4x4 MakeWorldTransform(const Vector3f& position, const Vector3f& rotation, float s)
{
	Matrix4x4 T = MakeTranslation(position);
	Matrix4x4 R = MakeRotation(rotation);
	Matrix4x4 S = MakeScale(s);
	return T * R * S; // 注意矩阵乘法的顺序
}

// 构造的新坐标系到世界坐标系的变换矩阵：
inline Matrix3x3 MakeCoordinateSystem(const Vector3f& w)
{
	// u, v
	Vector3f u(1.0f, 0.0f, 0.0f);

	if (glm::dot(w, u) > 0.99f)
	{
		u = Vector3f(0.0f, 1.0f, 0.0f);
	}

	Vector3f v = glm::cross(w, u);
	u = glm::cross(v, w);

	u = glm::normalize(u);
	v = glm::normalize(v);

	return Matrix3x3(u, v, w); // 列主序
}