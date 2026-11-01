#pragma once
#include <vector>
#include "Common.h"

class Texture
{
public:
	Texture(int width, int height, int channel, uint8_t* data)
        : mWidth(width), mHeight(height), mChannels(channel)
    {
        mData.resize(width * height * channel);
		memcpy(mData.data(), data, width * height * channel);
    }

	static Texture* CreateFromFile(const std::string& filePath);

	// float, Vector2f, Vector3f(Color), Vector4f
	template<typename T>
	T GetPixel(int x, int y)
	{
		// 钳位模式：
		//x = glm::clamp(x, 0, mWidth - 1);
		//y = glm::clamp(y, 0, mHeight - 1);

		// 重复模式：
		x = x % mWidth;
		if (x < 0)
			x += mWidth;

		y = y % mHeight;
		if (y < 0)
			y += mHeight;

		int index = (y * mWidth + x) * mChannels;

		if constexpr (std::is_same_v<T, float>)
		{
			return mData[index] / 255.0f;
		}
		else
		{
			T result(0);
			int count = std::min(T::length(), mChannels);
			for (int i = 0; i < count; i++)
			{
				result[i] = mData[index + i] / 255.0f;
			}
			return result;
		}
	}

	template<typename T>
	T Sample(Vector2f uv)
	{
		// 临近点采样
		// int x = std::round(uv.x * mWidth);
		// int y = std::round(uv.y * mHeight);

		// 线性插值采样
		float fx = uv.x * mWidth - 0.5f;
		float fy = uv.y * mHeight - 0.5f;

		int x = (int)std::floor(fx);
		int y = (int)std::floor(fy);

		float k1 = fx - x;
		float k2 = fy - y;

		T c1 = GetPixel<T>(x, y);
		T c2 = GetPixel<T>(x + 1, y);
		T c3 = GetPixel<T>(x, y + 1);
		T c4 = GetPixel<T>(x + 1, y + 1);

		T c5 = glm::mix(c1, c2, k1);
		T c6 = glm::mix(c3, c4, k1);
        return glm::mix(c5, c6, k2);
	}
private:
	int mWidth;
	int mHeight;
	int mChannels;
	std::vector<uint8_t> mData;
};

