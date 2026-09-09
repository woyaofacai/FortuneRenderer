#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext.hpp>
#include <random>

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
const float INV_PI = 1.0f / PI;

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

	if (fabs(glm::dot(w, u)) > 0.99f)
	{
		u = Vector3f(0.0f, 1.0f, 0.0f);
	}

	Vector3f v = glm::cross(w, u);
	u = glm::cross(v, w);

	u = glm::normalize(u);
	v = glm::normalize(v);

	return Matrix3x3(u, v, w); // 列主序
}

inline int RandomInt(int a, int b)
{
	thread_local std::mt19937 gen(std::random_device{}());
	std::uniform_int_distribution<int> dist(a, b); 
	return dist(gen);
}

inline float Random01()
{
	thread_local std::mt19937 gen(std::random_device{}());
	thread_local std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	return dist(gen);
}

inline float Random(float a, float b)
{
	return a + (b - a) * Random01();
}

// 从球坐标系转到笛卡尔坐标系：
inline Vector3f GetSphericalCoordinate(float theta, float phi)
{
	return Vector3f(
		sinf(theta) * cosf(phi),
		sinf(theta) * sinf(phi),
		cosf(theta)
	);
}


inline bool ComputeRefractVector(const Vector3f& wi, float eta_i, float eta_t, Vector3f& wt)
{
	float cos_i = wi.z;
    float sin_i = sqrtf(glm::max(0.0f, 1.0f - cos_i * cos_i));
	float sin_t = sin_i * eta_i / eta_t;
	
	// 全反射
	if (sin_t >= 1.0f)
	{
		return false;
	}

	float cos_t = sqrtf(1.0f - sin_t * sin_t);
    wt.z = wi.z > 0 ? -cos_t : cos_t;
	wt.x = -wi.x * sin_t / glm::max(1e-3f, sin_i);
    wt.y = -wi.y * sin_t / glm::max(1e-3f, sin_i);

	wt = glm::normalize(wt);
	return true;
}

// 在圆形内均匀采样：
inline Vector2f UniformSampleDisk(float R)
{
	float e1 = Random01();
	float e2 = Random01();

	float r = R * std::sqrt(e1);
	float theta = 2.0f * PI * e2;

	float x = r * std::cos(theta);
	float y = r * std::sin(theta);
	return Vector2f(x, y);
}

// 在单位半球面进行均匀采样 w:
inline Vector3f UniformSampleHemisphere()
{
	float e1 = Random01();
	float e2 = Random01();

	float cosTheta = e1;
	float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
	float phi = 2.0f * PI * e2;

	float x = sinTheta * std::cos(phi);
	float y = sinTheta * std::sin(phi);
	float z = cosTheta;

	return Vector3f(x, y, z);
}

// 在单位球面进行均匀采样 w:
inline Vector3f UniformSampleSphere()
{
	float e1 = Random01();
	float e2 = Random01();

	float phi = 2 * PI * e1;
	float costheta = 1 - 2 * e2;
	float sintheta = std::sqrt(std::max(0.0f, 1 - costheta * costheta));

	Vector3f p;
	p.x = sintheta * std::cos(phi);
	p.y = sintheta * std::sin(phi);
	p.z = costheta;
	return p;
}

// 均匀采样三角形
inline Vector3f UniformSampleTriangle(const Vector3f& p0, 
	const Vector3f& p1, 
	const Vector3f& p2)
{
	float e1 = Random01();
	float e2 = Random01();

	float sqrt_e1 = std::sqrt(e1);
	float u = 1.0f - sqrt_e1;
	float v = e2 * sqrt_e1;

	Vector3f p = (1 - u - v) * p0 + u * p1 + v * p2;
    return p;
}