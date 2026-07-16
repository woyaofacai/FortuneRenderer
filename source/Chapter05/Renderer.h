#pragma once
#include "Common.h"
#include <atomic>
#include "Camera.h"
#include "Sphere.h"
#include "Disk.h"
#include "Triangle.h"
#include <vector>
#include "Scene.h"

class Renderer
{
public:
	Renderer(int w, int h, int samplePerPixel, const char* filepath);
	virtual ~Renderer();
	void Run();

private:
	Color RenderPixel(int x, int y);
	Color RenderSubPixel(float x, float y);

	void RunRenderThread();

	int mViewportWidth = 800;
	int mViewportHeight = 600;
	// 屏幕上每个像素的采样次数（SPP）：
	int SamplePerPixel = 100;

	uint32_t* mBuffer = nullptr;

	std::atomic<int> mCurrentPixelIndex = 0;

	Scene* mScene = nullptr;
};

