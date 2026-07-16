#pragma once
#include "Ray.h"



class Sphere
{
public:
	Sphere(const Vector3f& center, float R);
	bool Intersect(Ray ray, Intersection& isect) const;

private:
	float		mRadius;
	Matrix4x4	mObjectToWorld; // 对象空间到世界空间的变换矩阵
	Matrix4x4	mWorldToObject; // 世界空间到对象空间的变换矩阵
};

