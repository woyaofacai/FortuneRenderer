#pragma once
#include "Primitive.h"



class Sphere : public Primitive
{
public:
	Sphere(SceneObject* pSceneObject, float R);
	virtual bool Intersect(Ray ray, Intersection& isect) const override;

private:
	float		mRadius;
};

