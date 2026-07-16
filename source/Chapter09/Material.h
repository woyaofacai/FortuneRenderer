#pragma once
#include "Ray.h"

class Material
{
public:
	virtual ~Material() = default;
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const = 0;
	virtual bool IsSpecular() const { return false; }
};

class LambertMaterial : public Material
{
public:
	LambertMaterial(const Color& albedo) : mAlbedo(albedo) {}
	
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const override
	{
		return mAlbedo * INV_PI;
	}

private:
	Color mAlbedo;
};
