#include "Disk.h"
#include "SceneObject.h"

Disk::Disk(SceneObject* pSceneObject, float radius)
	: Primitive(pSceneObject), mRadius(radius)
{
	
}

bool Disk::Intersect(Ray ray, Intersection& isect) const
{
	// ray转到圆盘的局部空间：
	Ray r = m_pSceneObject->GetWorldToObject() * ray;

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

	isect.position = Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(p, 1.0f)); // 交点位置（世界空间）
	isect.normal = glm::normalize(Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(0, 0, 1, 0))); // 圆盘的法线（世界空间）
	isect.t = t;
	return true;
}

void Disk::Sample(Vector3f& p, Vector3f& normal, float& pdf) const
{
	Vector2f p2 = UniformSampleDisk(mRadius);
	p = Vector3f(p2.x, p2.y, 0);

	// 转成世界坐标：
	p = Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(p, 1.0f));
    normal = glm::normalize(Vector3f(m_pSceneObject->GetObjectToWorld() * Vector4f(0, 0, 1, 0)));
    pdf = 1.0f / (PI * mRadius * mRadius);
}
