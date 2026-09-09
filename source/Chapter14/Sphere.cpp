#include "Sphere.h"
#include "Disk.h"
#include "SceneObject.h"

Sphere::Sphere(SceneObject* pSceneObject, float R)
	: Primitive(pSceneObject), mRadius(R)
{
	
}


bool Sphere::Intersect(Ray ray, Intersection& isect) const
{
	// ray转到球体的局部空间：
	Ray r = m_pSceneObject->GetWorldToObject() * ray;

	float A = glm::dot(r.d, r.d);
	float B = 2.0f * glm::dot(r.d, r.o);
	float C = glm::dot(r.o, r.o) - mRadius * mRadius;

	float delta = B * B - 4.0f * A * C;
	if (delta < 0.0f)
		return false;
	
	float sqrtDelta = sqrtf(delta);
	float t1 = (-B - sqrtDelta) / (2.0f * A);
	float t2 = (-B + sqrtDelta) / (2.0f * A);

	// t1 < t2:
	if (t2 < r.mint)
		return false;

	if (t1 > r.maxt)
		return false;

	float t = t1;
	if (t < r.mint)
	{
		t = t2;
		if (t > r.maxt)
			return false;
	}

	Vector3f p = r.o + t * r.d; // 交点位置（局部空间）
	Vector3f n = glm::normalize(p); // 交点法线（局部空间）

	isect.position = Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(p, 1.0f)); // 交点位置（世界空间）
	isect.normal = glm::normalize(Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(n, 0.0f))); // 交点法线（世界空间）
	isect.t = t;

	return true;
}

void Sphere::Sample(Vector3f& p, Vector3f& normal, float& pdf) const
{
	p = UniformSampleSphere();
	normal = p;
	p *= mRadius;

	// 转到世界空间：
	p = Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(p, 1.0f));
    normal = glm::normalize(Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(normal, 0.0f)));
    pdf = 1.0f / (4.0f * PI * mRadius * mRadius);
}

