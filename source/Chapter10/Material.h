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

class ConductorSpecularMaterial : public Material
{
public:
	ConductorSpecularMaterial(const Color& eta, const Color& absorptionCoef, const Color& reflectionColor)
		: mEta(eta), mAbsorptionCoef(absorptionCoef), mReflectionColor(reflectionColor) {}

	virtual bool IsSpecular() const { return true; }
	
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const override
	{
 		if (!(std::fabs(wo.x + wi.x) < 1e-4f 
			&& std::fabs(wo.y + wi.y) < 1e-4f 
			&& std::fabs(wo.z - wi.z) < 1e-4f))
		 {
             return Color(0.0f, 0.0f, 0.0f);
		 }

		float cosTheta = std::max(1e-5f, wi.z);
		Color k = mAbsorptionCoef;
        Color r1 = ((mEta * mEta + k * k) * cosTheta * cosTheta - 2.0f * mEta * cosTheta + 1.0f)
                / ((mEta * mEta + k * k) * cosTheta * cosTheta + 2.0f * mEta * cosTheta + 1.0f);

        Color r2 = (mEta * mEta + k * k - 2.0f * mEta * cosTheta + cosTheta * cosTheta)
                / (mEta * mEta + k * k + 2.0f * mEta * cosTheta + cosTheta * cosTheta);
        Color Fr = (r1 + r2) * 0.5f;

        return Fr * mReflectionColor / std::max(std::fabs(cosTheta), 1e-4f);
	}

private:
	Color mEta;
	Color mAbsorptionCoef;
	Color mReflectionColor;
};