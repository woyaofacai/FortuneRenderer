#include "Disk.h"

Disk::Disk(const Vector3f& center, const Vector3f& euler, float radius)
	: mRadius(radius)
{
	mObjectToWorld = MakeWorldTransform(center, euler, 1.0f);
	mWorldToObject = glm::inverse(mObjectToWorld);
}

bool Disk::Intersect(Ray ray, Intersection& isect) const
{
	// ray转到圆盘的局部空间：
	Ray r = mWorldToObject * ray;

	// t 
	if (fabs(r.d.z) < 1e-6f) // 射线与圆盘所在平面平行，无交点
		return false;

	float t = -r.o.z / r.d.z; // 圆盘所在平面的z=0

	if (t < r.mint || t > r.maxt) // 交点不在射线的有效范围内
		return false;
	
	Vector3f p = r.o + t * r.d; // 交点位置（局部空间）
	// 判断交点是否在圆盘内： | p | <= radius
	if (glm::dot(p, p) > mRadius * mRadius) // 交点在圆盘外
		return false;

	isect.position = Vector3f(mObjectToWorld * Vector4f(p, 1.0f)); // 交点位置（世界空间）
	isect.normal = glm::normalize(Vector3f(mObjectToWorld * Vector4f(0, 0, 1, 0))); // 圆盘的法线（世界空间）
	isect.t = t;
	return true;
}