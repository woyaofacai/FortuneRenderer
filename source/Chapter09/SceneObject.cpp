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

