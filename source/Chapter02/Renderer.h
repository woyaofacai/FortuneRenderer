#pragma once
#include "Common.h"
#include <atomic>

class Renderer
{
public:
	Renderer(int w, int h);
	virtual ~Renderer() {}
	void Run();

private:
	Color RenderPixel(int x, int y);
	void RunRenderThread();

	int mViewportWidth = 800;
	int mViewportHeight = 600;
	uint32_t* mBuffer = nullptr;

	std::atomic<int> mCurrentPixelIndex = 0;
};

