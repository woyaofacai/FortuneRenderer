#include "Renderer.h"
#include <MiniFB.h>
#include "Common.h"
#include "Material.h"
#include <thread>
#include <vector>

Renderer::Renderer(int w, int h, int minDepth, int maxDepth, int samplePerPixel, const char* filepath)
	: mViewportWidth(w)
    , mViewportHeight(h)
	, mMinDepth(minDepth)
    , mMaxDepth(maxDepth)
	, SamplePerPixel(samplePerPixel)
{
	mCurrentPixelIndex = 0;
	mScene = Scene::LoadSceneFromXML(filepath, w, h);
}

Renderer::~Renderer()
{
	if (mScene)
		delete mScene;
}

void Renderer::Run()
{
	struct mfb_window* window = mfb_open_ex("Fortune Renderer", mViewportWidth, mViewportHeight, MFB_WF_RESIZABLE);
	if (window == NULL)
		return;

	// 屏幕/窗口/视口上每个像素点的颜色，以32位整数表示，格式为0xAARRGGBB（Alpha, Red, Green, Blue）
	mBuffer = (uint32_t*)malloc(mViewportWidth * mViewportHeight * 4);

	std::thread renderThread(&Renderer::RunRenderThread, this);
	renderThread.detach();

	int numThreads = std::thread::hardware_concurrency();
	std::vector<std::thread> renderThreads(numThreads);
	for (int i = 0; i < numThreads; i++) 
	{
		renderThreads[i] = std::thread(&Renderer::RunRenderThread, this);
		renderThreads[i].detach();
	}

	// Present：
	mfb_update_state state;
	do {
		state = mfb_update_ex(window, mBuffer, mViewportWidth, mViewportHeight);

		if (state != MFB_STATE_OK)
			break;

	} while (mfb_wait_sync(window));

	free(mBuffer);
	mBuffer = NULL;
	window = NULL;
}

Color Renderer::RenderPixel(int x, int y)
{
	// SSAA
	const int N = SamplePerPixel; // 每个像素采样的次数
	Color resultColor(0, 0, 0);

	for (int i = 0; i < N; i++)
	{
		// (x, y) - (x+1, y+1)范围内随机采样一个点：
		float px = x + Random01();
		float py = y + Random01();

		Color color = RenderSubPixel(px, py);
		resultColor += (color / (float)N);
	}

	return resultColor; // 取平均值，得到最终颜色	
}

Color Renderer::RenderSubPixel(float x, float y)
{
	Ray ray = mScene->GetCamera().GetRay(x, y);
	Color color = GetRadiance(ray, 0, false);
	return color;
}

Color Renderer::GetRadiance(const Ray& ray, int depth, bool bPrevIsSpecular)
{
	if (depth > mMaxDepth)
		return Color(0, 0, 0);

	// 俄罗斯轮盘
	static const float SurvivalProbability = 0.8f;
	float RewardFactor = 1.0f;
	if (depth >= mMinDepth)
	{
		float K = Random01();
		if (K > SurvivalProbability)
		{
			return Color(0, 0, 0);
		}
		RewardFactor = 1.0f / SurvivalProbability;
	}

	Intersection isect;
	SceneObject* pSceneObject = mScene->Intersect(ray, isect);
	if (pSceneObject == nullptr)
		return Color(0, 0, 0);

	Material* pMaterial = pSceneObject->GetMaterial();
	Color Lo(0, 0, 0);

	Matrix3x3 localToWorld = MakeCoordinateSystem(isect.normal);
	Matrix3x3 worldToLocal = glm::transpose(localToWorld);

	Vector3f wo = worldToLocal * (-ray.d); // 出射方向，转换到局部坐标系

	// 自发光：
	Color Le = pSceneObject->GetEmissive();
	if (bPrevIsSpecular || depth == 0)
	{
		Lo += Le;
	}

	// 直接光照：
	if (!pMaterial->IsSpecular())
	{
		for (Light* pLight : mScene->GetLights())
		{
			Vector3f sourcePos;
			float pdf;
			Color L = pLight->GetRadiance(isect.position, sourcePos, pdf);

			// 求shadowRay
			Ray shadowRay;
			shadowRay.o = isect.position;
			shadowRay.d = glm::normalize(sourcePos - isect.position);
			shadowRay.mint = 1e-3f;
			shadowRay.maxt = glm::length(sourcePos - isect.position) - 1e-3f;

			Intersection shadow_isect;
			if (mScene->Intersect(shadowRay, shadow_isect)) // 如果shadowRay与场景中的物体相交，说明该点被遮挡了
				continue;

			Vector3f wi = worldToLocal * shadowRay.d; // 入射方向，转换到局部坐标系
			float cosTheta = glm::dot(isect.normal, shadowRay.d);
			Color brdf = pMaterial->BRDF(wo, wi);
			Lo += brdf * L * glm::max(cosTheta, 0.0f) / std::max(1e-5f, pdf);
		}
	}

	// 间接光照：
	
	// 反射：
	{
		float pdf;
		Vector3f wi = pMaterial->SampleWi(wo, pdf);
		Color brdf = pMaterial->BRDF(wo, wi);
		if (IsGreaterThanZero(brdf))
		{
			Ray r;
			r.d = localToWorld * wi;
			r.o = isect.position;
			r.mint = 1e-3f;
			Color Li = GetRadiance(r, depth + 1, pMaterial->IsSpecular());
			Lo += brdf * Li * fabs(wi.z) / std::max(1e-5f, pdf);
		}
	}
	
	// 折射：
	Vector3f wt;
	if (pMaterial->SampleWt(wo, wt))
	{
		Color btdf = pMaterial->BTDF(wo, wt);
		if (IsGreaterThanZero(btdf))
		{
			Ray r;
			r.d = localToWorld * wt;
			r.o = isect.position;
			r.mint = 1e-3f;
			
			Color Li = GetRadiance(r, depth + 1, pMaterial->IsSpecular());
			Lo += btdf * Li * fabs(wt.z);
		}
	}

	return Lo * RewardFactor;
}

// 渲染线程的入口函数，负责执行渲染循环
void Renderer::RunRenderThread()
{
	while (true)
	{
		// 读取当前屏幕的下一个像素
		int pixelIndex = mCurrentPixelIndex.fetch_add(1);
		if (pixelIndex >= mViewportWidth * mViewportHeight)
			break;

		int x = pixelIndex % mViewportWidth;
		int y = pixelIndex / mViewportWidth;

		Color color = RenderPixel(x, y);
		uint32_t r = glm::clamp((uint32_t)std::round(color.r * 255.0f), 0u, 255u);
		uint32_t g = glm::clamp((uint32_t)std::round(color.g * 255.0f), 0u, 255u);
		uint32_t b = glm::clamp((uint32_t)std::round(color.b * 255.0f), 0u, 255u);
		mBuffer[y * mViewportWidth + x] = (r << 16) | (g << 8) | (b);
	}
}

