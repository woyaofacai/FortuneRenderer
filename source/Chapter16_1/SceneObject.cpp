#include "SceneObject.h"

bool SceneObject::Intersect(Ray ray, Intersection& isect) const
{
	bool hit = false;
	for (const auto& primitive : mPrimitives)
	{
		if (primitive->Intersect(ray, isect))
		{
			ray.maxt = isect.t;
			hit = true;
		}
	}
	return hit;
}

SceneObject::~SceneObject()
{
	for (auto& primitive : mPrimitives)
	{
		if (primitive)
			delete primitive;
	}
}

void SceneObject::Sample(Vector3f& p, Vector3f& normal, float& pdf)
{
	// 随机取一个Primitive
	int index = RandomInt(0, mPrimitives.size() - 1);
    mPrimitives[index]->Sample(p, normal, pdf);
	pdf /= mPrimitives.size();
}
