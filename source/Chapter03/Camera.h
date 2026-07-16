#pragma once
#include "Ray.h"

class Camera
{
public:
	void Initialize(const Vector3f& p, const Vector3f& target, const Vector3f& up, 
		float fov, float n, float f, int W, int H);

	Ray GetRay(int x, int y) const;

private:
	Vector3f mPosition;
	Matrix4x4 mCombinedMatrix;
	Matrix4x4 mInvCombinedMatrix;
};

