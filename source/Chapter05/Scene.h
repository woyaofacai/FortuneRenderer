#pragma once
#include "SceneObject.h"
#include "Camera.h"

class Scene
{
public:
	static Scene* LoadSceneFromXML(const char* filepath, int W, int H);

	~Scene();
	void SetCamera(const Camera& camera) { mCamera = camera; }
	const Camera& GetCamera() const { return mCamera; }
	SceneObject* Intersect(Ray ray, Intersection& isect) const;
	SceneObject* CreateSceneObject(const Vector3f& position, const Vector3f& euler, float scale);
	

private:
	Camera	mCamera;
	std::vector<SceneObject*> mSceneObjects;

};

