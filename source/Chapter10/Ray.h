#pragma once
#include "Common.h"

struct Ray
{
	Vector3f o;
	Vector3f d;

	float mint = 0.0f;
	float maxt = FLT_MAX;
};

inline Ray operator*(const Matrix4x4& m, const Ray& r)
{
	Ray result;
	result.o = Vector3f(m * Vector4f(r.o, 1.0f));
	result.d = Vector3f(m * Vector4f(r.d, 0.0f));
	result.mint = r.mint;
	result.maxt = r.maxt;
	return result;
}

struct Intersection
{
	Vector3f position; // 交点位置
	Vector3f normal;   // 交点法线
	float t;          // 射线参数t值，即交点到射线原点的距离
};

