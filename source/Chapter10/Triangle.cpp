#include "Triangle.h"
#include "SceneObject.h"

Triangle::Triangle(SceneObject* pSceneObject, const Vector3f& v0, const Vector3f& v1, const Vector3f& v2)
	: Primitive(pSceneObject)
{
	Matrix4x4 worldMatrix = pSceneObject->GetObjectToWorld();

	mVertices[0] = Vector3f(worldMatrix * Vector4f(v0, 1.0f));
	mVertices[1] = Vector3f(worldMatrix * Vector4f(v1, 1.0f));
	mVertices[2] = Vector3f(worldMatrix * Vector4f(v2, 1.0f));

	// 计算三角形的法线：
	Vector3f edge1 = mVertices[1] - mVertices[0];
	Vector3f edge2 = mVertices[2] - mVertices[0];
	mNormal = glm::normalize(glm::cross(edge1, edge2));
}

bool Triangle::Intersect(Ray ray, Intersection& isect) const
{
	Vector3f p0 = mVertices[0];
	Vector3f p1 = mVertices[1];
	Vector3f p2 = mVertices[2];

	Vector3f e1 = p1 - p0;
	Vector3f e2 = p2 - p0;
	Vector3f s = ray.o - p0;

	Vector3f s1 = cross(ray.d, e2);
	Vector3f s2 = cross(s, e1);

	float det = dot(s1, e1);
	if (fabs(det) < 1e-6f) // 射线与三角形所在平面平行，无交点
		return false;

	float invDet = 1.0f / det;
	float b1 = dot(s1, s) * invDet;
	float b2 = dot(s2, ray.d) * invDet;
	float t = dot(s2, e2) * invDet;

	if (t < ray.mint || t > ray.maxt) // 交点不在射线的有效范围内
		return false;

	float b0 = 1.0f - b1 - b2;
	if (b1 < 0.0f || b2 < 0.0f || b0 < 0.0f) // 交点在三角形外
		return false;

	isect.position = ray.o + t * ray.d; // 交点位置
	isect.normal = mNormal; // 交点法线
	isect.t = t;
	return true;
}
