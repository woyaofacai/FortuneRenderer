#pragma once
#include "Primitive.h"
#include "SceneObject.h"

class Disk : public Primitive
{
public:
	//Disk(const Vector3f& center, const Vector3f& euler, float radius);
	Disk(SceneObject* pSceneObject, float radius);
	virtual bool Intersect(Ray ray, Intersection& isect) const override;
	virtual void Sample(Vector3f& p, Vector3f& normal, float& pdf) const override;
private:
	float	mRadius;
};

