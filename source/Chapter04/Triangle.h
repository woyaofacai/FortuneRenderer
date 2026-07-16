#pragma once
#include "Ray.h"
class Triangle
{
public:
	Triangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Matrix4x4& worldMatrix);
	bool Intersect(Ray ray, Intersection& isect) const;

private:
	// 世界坐标系下的三角形顶点位置和法线：
	Vector3f mVertices[3]; // 三角形的三个顶点
	Vector3f mNormal; // 三角形的法线
};

