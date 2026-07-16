#pragma once
#include <map>
#include "SceneObject.h"
#include "Camera.h"
#include "Light.h"

class Scene
{
public:
	static Scene* LoadSceneFromXML(const char* filepath, int W, int H);

	~Scene();
	void SetCamera(const Camera& camera) { mCamera = camera; }
	const Camera& GetCamera() const { return mCamera; }
	SceneObject* Intersect(Ray ray, Intersection& isect) const;
	SceneObject* CreateSceneObject(const Vector3f& position, const Vector3f& euler, float scale);

	template<typename T, typename... Args>
	T* CreateLight(Args&&...args)
	{
		T* light = new T(std::forward<Args>(args)...);
		mLights.push_back(light);
		return light;
	}

	template<typename T, typename... Args>
	T* CreateMaterial(const std::string& name, Args&&... args)
	{
		T* material = new T(std::forward<Args>(args)...);
		mMaterials.insert({ name, material });
		return material;
	}

	Material* GetMaterial(const std::string& name) const
	{
		auto it = mMaterials.find(name);
		return it != mMaterials.end() ? it->second : nullptr;
	}

	const std::vector<Light*>& GetLights() const { return mLights; }

private:
	Camera	mCamera;
	std::vector<SceneObject*> mSceneObjects;
	std::vector<Light*>		mLights;
	std::map<std::string, Material*> mMaterials;
};

