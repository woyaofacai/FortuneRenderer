#define _CRT_SECURE_NO_WARNINGS

#include "Scene.h"
#include "Disk.h"
#include "Sphere.h"
#include "Triangle.h"
#include "tinyxml2.h"
#include <cstdio>

// 解析 "x, y, z" 格式的字符串为 Vector3f
static Vector3f ParseVector3f(const char* str)
{
	Vector3f v(0.0f);
	if (!str) return v;

	if (std::sscanf(str, " %f , %f , %f", &v.x, &v.y, &v.z) != 3) {
		// 解析失败：可选择保持默认值或报错处理
		v = Vector3f(0.0f);
	}
	return v;
}

// 解析 "x, y" 格式的字符串为 Vector2f
static Vector2f ParseVector2f(const char* str)
{
	Vector2f v(0.0f);
	if (!str) return v;

	if (std::sscanf(str, " %f , %f", &v.x, &v.y) != 2) {
		// 解析失败：可选择保持默认值或报错处理
		v = Vector2f(0.0f);
	}
	return v;
}

static float GetChildFloat(tinyxml2::XMLElement* parent, const char* childName, float defaultVal)
{
	tinyxml2::XMLElement* child = parent->FirstChildElement(childName);
	if (child && child->GetText())
		return (float)atof(child->GetText());
	return defaultVal;
}

static const char* GetChildText(tinyxml2::XMLElement* parent, const char* childName)
{
	tinyxml2::XMLElement* child = parent->FirstChildElement(childName);
	if (child)
		return child->GetText();
	return nullptr;
}

Scene* Scene::LoadSceneFromXML(const char* filepath, int W, int H)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filepath) != tinyxml2::XML_SUCCESS)
		return nullptr;

	tinyxml2::XMLElement* pRoot = doc.FirstChildElement("Scene");
	if (!pRoot)
		return nullptr;

	Scene* pScene = new Scene();

	// 解析 Camera
	tinyxml2::XMLElement* pCameraElem = pRoot->FirstChildElement("Camera");
	if (pCameraElem)
	{
		Vector3f position = ParseVector3f(GetChildText(pCameraElem, "Position"));
		Vector3f target = ParseVector3f(GetChildText(pCameraElem, "Target"));
		Vector3f up = ParseVector3f(GetChildText(pCameraElem, "Up"));
		float nearZ = GetChildFloat(pCameraElem, "NearZ", 0.1f);
		float farZ = GetChildFloat(pCameraElem, "FarZ", 1000.0f);
		float fovDeg = GetChildFloat(pCameraElem, "Fov", 45.0f);
		float fovRad = glm::radians(fovDeg);

		Camera camera;
        camera.Initialize(position, target, up, fovRad, nearZ, farZ, W, H);
		pScene->SetCamera(camera);
	}

	// 解析 SceneObjects
	tinyxml2::XMLElement* pSceneObjectsElem = pRoot->FirstChildElement("SceneObjects");
	if (pSceneObjectsElem)
	{
		for (tinyxml2::XMLElement* pObjElem = pSceneObjectsElem->FirstChildElement("SceneObject");
			pObjElem != nullptr;
			pObjElem = pObjElem->NextSiblingElement("SceneObject"))
		{
			SceneObject* pSceneObject = nullptr;

			// 解析 Transform
			tinyxml2::XMLElement* pTransformElem = pObjElem->FirstChildElement("Transform");
			if (pTransformElem)
			{
				Vector3f position = ParseVector3f(GetChildText(pTransformElem, "Position"));
				Vector3f rotation = ParseVector3f(GetChildText(pTransformElem, "Rotation"));
				float scale = GetChildFloat(pTransformElem, "Scale", 1.0f);

				// rotation 从角度转弧度
				rotation = glm::radians(rotation);
				pSceneObject = pScene->CreateSceneObject(position, rotation, scale);
			}

			if (!pSceneObject)
				continue;

			// 解析 Primitives
			tinyxml2::XMLElement* pPrimitivesElem = pObjElem->FirstChildElement("Primitives");
			if (pPrimitivesElem)
			{
				// Sphere
				for (tinyxml2::XMLElement* pElem = pPrimitivesElem->FirstChildElement("Sphere");
					pElem != nullptr;
					pElem = pElem->NextSiblingElement("Sphere"))
				{
					float radius = GetChildFloat(pElem, "Radius", 1.0f);
					pSceneObject->CreatePrimitive<Sphere>(radius);
				}

				// Disk
				for (tinyxml2::XMLElement* pElem = pPrimitivesElem->FirstChildElement("Disk");
					pElem != nullptr;
					pElem = pElem->NextSiblingElement("Disk"))
				{
					float radius = GetChildFloat(pElem, "Radius", 1.0f);
					pSceneObject->CreatePrimitive<Disk>(radius);
				}

				// Triangle
				for (tinyxml2::XMLElement* pElem = pPrimitivesElem->FirstChildElement("Triangle");
					pElem != nullptr;
					pElem = pElem->NextSiblingElement("Triangle"))
				{
					// 按顺序读取三个 Vertex
					tinyxml2::XMLElement* pV0 = pElem->FirstChildElement("Vertex");
					tinyxml2::XMLElement* pV1 = pV0 ? pV0->NextSiblingElement("Vertex") : nullptr;
					tinyxml2::XMLElement* pV2 = pV1 ? pV1->NextSiblingElement("Vertex") : nullptr;

					if (pV0 && pV1 && pV2)
					{
						Vector3f v0 = ParseVector3f(pV0->GetText());
						Vector3f v1 = ParseVector3f(pV1->GetText());
						Vector3f v2 = ParseVector3f(pV2->GetText());
						pSceneObject->CreatePrimitive<Triangle>(v0, v1, v2);
					}
				}
			}
		}
	}

	return pScene;
}
