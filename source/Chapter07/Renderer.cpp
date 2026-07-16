#include "Renderer.h"
#include <MiniFB.h>
#include "Common.h"
#include <thread>
#include <vector>

Renderer::Renderer(int w, int h, int samplePerPixel, const char* filepath)
	: mViewportWidth(w)
	, mViewportHeight(h)
	, SamplePerPixel(samplePerPixel)
{
	mCurrentPixelIndex = 0;
	mScene = Scene::LoadSceneFromXML(filepath, w, h);

	//mScene->CreateLight<DirectionalLight>(Vector3f(-1, -1, -1), Color(1.0f, 0, 0));
	//mScene->CreateLight<PointLight>(Vector3f(0, 5, 0), Color(0, 1.0f, 0), Vector3f(1.0f, 0.0f, 0.0f));
	
	//mScene = new Scene();

	//// 设置摄像机
	//Camera camera;
	//camera.Initialize(
	//	Vector3f(0, 0, 0), // 相机位置
	//	Vector3f(0, 0, 1), // 目标位置
	//	Vector3f(0.0f, 1.0f, 0.0f), // 上向量
	//	glm::radians(60.0f), // FOV
	//	0.1f, // 近裁剪面
	//	1000.0f, // 远裁剪面
	//	w, h // 视口宽高
	//);
	//mScene->SetCamera(camera);

	//// 给场景添加物体：

	//SceneObject* pSceneObject = mScene->CreateSceneObject(Vector3f(0, 0, 5), Vector3f(0, 0, 0), 2.0f);
	//pSceneObject->CreatePrimitive<Triangle>(Vector3f(-1, -1, 0), Vector3f(1, -1, 0), Vector3f(1, 1, 0));
	//pSceneObject->CreatePrimitive<Triangle>(Vector3f(-1, -1, 0), Vector3f(1, 1, 0), Vector3f(-1, 1, 0));

	//SceneObject* pSceneObject2 = mScene->CreateSceneObject(Vector3f(0, 0, 2), Vector3f(0, 0, 0), 1.0f);
	//pSceneObject2->CreatePrimitive<Sphere>(0.5f);


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

	int numThreads = std::thread::hardware_concurrency() - 8;
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
		float px = x + glm::linearRand(0.0f, 1.0f);
		float py = y + glm::linearRand(0.0f, 1.0f);

		Color color = RenderSubPixel(px, py);
		resultColor += (color / (float)N);
	}

	return resultColor; // 取平均值，得到最终颜色	
}

Color Renderer::RenderSubPixel(float x, float y)
{
	Ray ray = mScene->GetCamera().GetRay(x, y);
	Color color = GetIrradiance(ray);
	return color;
}

Color Renderer::GetIrradiance(const Ray& ray)
{
	Intersection isect;
	if (!mScene->Intersect(ray, isect))
		return Color(0, 0, 0);

	Color E(0, 0, 0);

	// E(p)
	for (Light* pLight : mScene->GetLights())
	{
		Vector3f sourcePos;
		Color L = pLight->GetRadiance(isect.position, sourcePos);

		// 求shadowRay
		Ray shadowRay;
		shadowRay.o = isect.position;
		shadowRay.d = glm::normalize(sourcePos - isect.position);
		shadowRay.mint = 1e-3f;
		shadowRay.maxt = glm::length(sourcePos - isect.position);

		Intersection shadow_isect;
		if (mScene->Intersect(shadowRay, shadow_isect)) // 如果shadowRay与场景中的物体相交，说明该点被遮挡了
			continue;

		float cosTheta = glm::dot(isect.normal, shadowRay.d);
		E += L * glm::max(cosTheta, 0.0f);
	}

	return E;
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

