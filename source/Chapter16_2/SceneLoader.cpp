#define _CRT_SECURE_NO_WARNINGS

#include "Scene.h"
#include "Disk.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Material.h"
#include "tinyxml2.h"
#include <cstdio>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <filesystem>

// 解析 "x, y, z" 格式的字符串为 Vector3f
static Vector3f ParseVector3f(const char* str)
{
	Vector3f v(0.0f);
	if (!str) return v;

	if (std::sscanf(str, " %f , %f , %f", &v.x, &v.y, &v.z) != 3) {
		// 解析失败，可选择保持默认值或报错处理
		v = Vector3f(0.0f);
	}
	return v;
}


// 将str中的字符串解析成浮点数数组，例如，如果str的内容是：3, -2.5, 3,5, 6,700
// 需要解析成：[3, -2.5, 3, 5, 6, 700]
// 返回实际解析出来的浮点数个数
static int ParseFloatArray(const char* str, float arr[])
{
	if (!str || !arr)
		return 0;

	int count = 0;
	const char* p = str;

	while (true)
	{
		// 跳过分隔符（空白、逗号）
		while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' || *p == ',')
			++p;

		if (*p == '\0')
			break;

		char* pEnd = nullptr;
		float value = std::strtof(p, &pEnd);

		// 当前位置无法解析出数字，说明遇到非法字符，停止解析
		if (pEnd == p)
			break;

		arr[count++] = value;
		p = pEnd;
	}

	return count;
}

static void ParseVertex(const char* str, Vector3f& position, Vector2f& uv)
{
	float arr[5];
	int count = ParseFloatArray(str, arr);
	position.x = (count > 0) ? arr[0] : 0;
	position.y = (count > 1) ? arr[1] : 0;
    position.z = (count > 2) ? arr[2] : 0;
    uv.x = (count > 3) ? arr[3] : 0;
    uv.y = (count > 4) ? arr[4] : 0;
}

// 解析 "x, y" 格式的字符串为 Vector2f
static Vector2f ParseVector2f(const char* str)
{
	Vector2f v(0.0f);
	if (!str) return v;

	if (std::sscanf(str, " %f , %f", &v.x, &v.y) != 2) {
		// 解析失败，可选择保持默认值或报错处理
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

Scene* Scene::LoadSceneFromXML(const char* xmlFilePath, int W, int H)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(xmlFilePath) != tinyxml2::XML_SUCCESS)
		return nullptr;

	tinyxml2::XMLElement* pRoot = doc.FirstChildElement("Scene");
	if (!pRoot)
		return nullptr;

	Scene* pScene = new Scene();

	auto GetOrCreateTexture = [xmlFilePath, pScene](tinyxml2::XMLElement* pMatElem, const char* propertyName)->Texture* {
		std::string dirPath = std::filesystem::path(xmlFilePath).parent_path().string();
		const char* textureFileName = GetChildText(pMatElem, propertyName);
		if (textureFileName == nullptr)
			return nullptr;

		std::string textureFilePath = dirPath + "/texture/" + textureFileName;
		return pScene->GetOrCreateTexture(textureFilePath);
	};

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

	// 解析 Materials（必须在 SceneObjects 之前，以便后续按名字引用）
	tinyxml2::XMLElement* pMaterialsElem = pRoot->FirstChildElement("Materials");
	if (pMaterialsElem)
	{
		for (tinyxml2::XMLElement* pMatElem = pMaterialsElem->FirstChildElement("Material");
			pMatElem != nullptr;
			pMatElem = pMatElem->NextSiblingElement("Material"))
		{
			const char* nameText = GetChildText(pMatElem, "Name");
			const char* typeText = GetChildText(pMatElem, "Type");
			if (!nameText || !typeText)
				continue;

			std::string name = nameText;
			std::string type = typeText;

			if (type == "Lambert")
			{
				Color albedo = ParseVector3f(GetChildText(pMatElem, "Albedo"));
				Texture* pAlbedoMap = GetOrCreateTexture(pMatElem, "AlbedoMap");
				pScene->CreateMaterial<LambertMaterial>(name, albedo, pAlbedoMap);
			}
			// 后续可在此扩展更多材质类型
			else if (type == "ConductorSpecular")
			{
				Color eta = ParseVector3f(GetChildText(pMatElem, "Eta"));
				Color absorptionCoef = ParseVector3f(GetChildText(pMatElem, "AbsorptionCoef"));
				Color reflectionColor = ParseVector3f(GetChildText(pMatElem, "ReflectionColor"));
				pScene->CreateMaterial<ConductorSpecularMaterial>(name, eta, absorptionCoef, reflectionColor);
			}
			else if (type == "DielectricSpecular")
			{
				float eta = GetChildFloat(pMatElem, "Eta", 1.5f);
				Color transmissionColor = ParseVector3f(GetChildText(pMatElem, "TransmissionColor"));
				pScene->CreateMaterial<DielectricSpecularMaterial>(name, eta, transmissionColor);
			}
			else if (type == "OrenNayar")
			{
				Color albedo = ParseVector3f(GetChildText(pMatElem, "Albedo"));
				Texture* pAlbedoMap = GetOrCreateTexture(pMatElem, "AlbedoMap");
				float roughness = GetChildFloat(pMatElem, "Sigma", 0.0f);
				pScene->CreateMaterial<OrenNayarMaterial>(name, albedo, pAlbedoMap, roughness);
			}
			else if (type == "TorranceSparrow")
			{
				float exponent = GetChildFloat(pMatElem, "Exponent", 1.0f);
				Color eta = ParseVector3f(GetChildText(pMatElem, "Eta"));
				Color absorptionCoef = ParseVector3f(GetChildText(pMatElem, "AbsorptionCoef"));
				Color reflectionColor = ParseVector3f(GetChildText(pMatElem, "ReflectionColor"));
				pScene->CreateMaterial<TorranceSparrowMaterial>(name, exponent, eta, absorptionCoef, reflectionColor);
			}
		}
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

			// 解析 Material 引用（按材质名查找）
			const char* materialName = GetChildText(pObjElem, "Material");
			if (materialName)
			{
				Material* pMaterial = pScene->GetMaterial(materialName);
				if (pMaterial)
					pSceneObject->SetMaterial(pMaterial);
			}

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
						Vector3f positions[3];
						Vector2f uvs[3];

						ParseVertex(pV0->GetText(), positions[0], uvs[0]);
						ParseVertex(pV1->GetText(), positions[1], uvs[1]);
						ParseVertex(pV2->GetText(), positions[2], uvs[2]);
						pSceneObject->CreatePrimitive<Triangle>(positions, uvs);
					}
				}
			}

			// 读取Emissive并设置
			const char* emissiveText = GetChildText(pObjElem, "Emissive");
            if (emissiveText)
            {
                Color emissive = ParseVector3f(emissiveText);
                pSceneObject->SetEmissive(emissive);
				if (emissive.r > 1e-5f || emissive.g > 1e-5f || emissive.b > 1e-6f)
				{
					pScene->CreateLight<AreaLight>(pSceneObject);
				}
            }
		}
	}

	// 解析 Lights
	tinyxml2::XMLElement* pLightsElem = pRoot->FirstChildElement("Lights");
	if (pLightsElem)
	{
		// DirectionalLight
		for (tinyxml2::XMLElement* pElem = pLightsElem->FirstChildElement("DirectionalLight");
			pElem != nullptr;
			pElem = pElem->NextSiblingElement("DirectionalLight"))
		{
			Vector3f direction = ParseVector3f(GetChildText(pElem, "Direction"));
			Color    radiance  = ParseVector3f(GetChildText(pElem, "Radiance"));
			pScene->CreateLight<DirectionalLight>(direction, radiance);
		}

		// PointLight
		for (tinyxml2::XMLElement* pElem = pLightsElem->FirstChildElement("PointLight");
			pElem != nullptr;
			pElem = pElem->NextSiblingElement("PointLight"))
		{
			Vector3f position     = ParseVector3f(GetChildText(pElem, "Position"));
			Color    intensity    = ParseVector3f(GetChildText(pElem, "Intensity"));
			Vector3f attenuations = ParseVector3f(GetChildText(pElem, "Attenuations"));
			pScene->CreateLight<PointLight>(position, intensity, attenuations);
		}

		// SpotLight
		for (tinyxml2::XMLElement* pElem = pLightsElem->FirstChildElement("SpotLight");
			pElem != nullptr;
			pElem = pElem->NextSiblingElement("SpotLight"))
		{
			Vector3f position     = ParseVector3f(GetChildText(pElem, "Position"));
			Vector3f direction    = ParseVector3f(GetChildText(pElem, "Direction"));
			Color    intensity    = ParseVector3f(GetChildText(pElem, "Intensity"));
			float    innerAngle   = GetChildFloat(pElem, "InnerAngle", 0.0f);
			float    outerAngle   = GetChildFloat(pElem, "OuterAngle", 0.0f);
			Vector3f attenuations = ParseVector3f(GetChildText(pElem, "Attenuations"));

			// XML 中以"度"为单位，转换为弧度
			float innerRad = glm::radians(innerAngle);
			float outerRad = glm::radians(outerAngle);

			pScene->CreateLight<SpotLight>(position, direction, intensity, innerRad, outerRad, attenuations);
		}
	}

	return pScene;
}
