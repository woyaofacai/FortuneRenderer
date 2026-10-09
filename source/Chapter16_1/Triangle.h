#pragma once
#include "Primitive.h"

class Triangle : public Primitive
{
public:
	Triangle(SceneObject* pSceneObject, 
		Vector3f positions[],
		Vector2f uvs[]);

	virtual bool Intersect(Ray ray, Intersection& isect) const override;

	virtual void Sample(Vector3f& p, Vector3f& normal, float& pdf) const override;

private:
	// 世界坐标系下的三角形顶点位置和法线：
	Vector3f mPositions[3]; // 三角形的三个顶点
	Vector2f mTexCoords[3]; // 三角形的三个顶点的UV
	Vector3f mNormal; // 三角形的法线
	float mArea;
};

