#include "Camera.h"
void Camera::Initialize(const Vector3f& p, const Vector3f& target, const Vector3f& up,
	float fov, float n, float f, int W, int H)
{
	mPosition = p;

	// 求观察矩阵：
	//Vector3f l = glm::normalize(target - p); // 观察方向（从相机位置指向目标位置的向量）
	//Vector3f r = glm::normalize(glm::cross(up, l)); 
	//Vector3f u = glm::cross(l, r);

	//Matrix4x4 viewMatrix = glm::transpose(Matrix4x4(
	//	r.x, r.y, r.z, 0.0f,
	//	u.x, u.y, u.z, 0.0f,
	//	l.x, l.y, l.z, 0.0f,
	//	0.0f, 0.0f, 0.0f, 1.0f
	//)) * 
	//MakeTranslation(-p); // 注意矩阵乘法的顺序

	// left-hand: 左手坐标系：
	Matrix4x4 viewMatrix = glm::lookAtLH(p, target, up);

	// 求投影矩阵：
	Matrix4x4 projectionMatrix = glm::perspectiveFovLH_ZO(fov, (float)W, (float)H, n, f);

	// 求视口矩阵：
	Matrix4x4 viewportMatrix = Matrix4x4(
		W / 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -H / 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		W / 2.0f, H / 2.0f, 0.0f, 1.0f
	);

	Matrix4x4 combinedMatrix = viewportMatrix * projectionMatrix * viewMatrix; // 注意矩阵乘法的顺序
	Matrix4x4 invCombinedMatrix = glm::inverse(combinedMatrix);

	mCombinedMatrix = combinedMatrix;
	mInvCombinedMatrix = invCombinedMatrix;
}

Ray Camera::GetRay(int x, int y) const
{
	Ray ray;
	ray.o = mPosition;

	Vector4f p(x, y, 0.0f, 1.0f);
	Vector4f worldPos = mInvCombinedMatrix * p; 
	worldPos /= worldPos.w; // 齐次坐标除以w分量，得到世界坐标
	ray.d = glm::normalize(Vector3f(worldPos) - mPosition); // 从相机位置指向世界坐标的方向向量

	return ray;
}
