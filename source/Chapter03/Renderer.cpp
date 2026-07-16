#include "Renderer.h"
#include <MiniFB.h>
#include "Common.h"
#include <thread>
#include <vector>

Renderer::Renderer(int w, int h)
	: mViewportWidth(w), 
	mViewportHeight(h)
{
	mCurrentPixelIndex = 0;

	mCamera.Initialize(
		Vector3f(0, 0, 0), // 相机位置
		Vector3f(0, 0, 1), // 目标位置
		Vector3f(0.0f, 1.0f, 0.0f), // 上向量
		glm::radians(60.0f), // FOV
		0.1f, // 近裁剪面
		1000.0f, // 远裁剪面
		w, h // 视口宽高
	);
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
	// 本节课重点：GetRay()函数：根据像素坐标(x, y)计算出从相机位置出发、穿过该像素所在位置的射线（Ray），以便后续进行光线追踪等渲染计算。
	Ray ray = mCamera.GetRay(x, y);

	// 把ray.d打印到屏幕上：
	Color color = ray.d * 0.5f + 0.5f;
	return color;
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

