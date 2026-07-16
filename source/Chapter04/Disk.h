#pragma once
#include "Ray.h"
class Disk
{
public:
	Disk(const Vector3f& center, const Vector3f& euler, float radius);
	bool Intersect(Ray ray, Intersection& isect) const;

private:
	float	mRadius;
	Matrix4x4	mObjectToWorld; // 对象空间到世界空间的变换矩阵
	Matrix4x4	mWorldToObject; // 世界空间到对象空间的变换矩阵
};

