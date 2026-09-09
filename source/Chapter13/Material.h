#pragma once
#include "Ray.h"

class Material
{
public:
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const = 0;
	virtual Color BTDF(const Vector3f& wt, const Vector3f& wi) const { return Color(0); }
	virtual ~Material() = default;
	virtual bool IsSpecular() const { return false; }
	virtual bool SampleWt(const Vector3f& wo, Vector3f& wt) const { return false; }
    virtual Vector3f SampleWi(const Vector3f& wo, float& pdf) const { return Vector3f(0); }
};

class LambertMaterial : public Material
{
public:
	LambertMaterial(const Color& albedo) : mAlbedo(albedo) {}
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const override;
	virtual Vector3f SampleWi(const Vector3f& wo, float& pdf) const override;
private:
	Color mAlbedo;
};

// 导体的镜面材质
class ConductorSpecularMaterial : public Material
{
public:
	ConductorSpecularMaterial(const Color& eta, const Color& absorptionCoef, const Color& reflectionColor)
		: mEta(eta), mAbsorptionCoef(absorptionCoef), mReflectionColor(reflectionColor) {}

	virtual bool IsSpecular() const { return true; }
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const override;
	virtual Vector3f SampleWi(const Vector3f& wo, float& pdf) const override;
private:
	Color mEta;
	Color mAbsorptionCoef;
	Color mReflectionColor;
};

// 绝缘体的镜面材质
class DielectricSpecularMaterial : public Material
{
public:
	DielectricSpecularMaterial(float eta, Color transmissionColor) : 
        mEta(eta), mTransmissionColor(transmissionColor) {}
	virtual bool IsSpecular() const { return true; }
	virtual Color BRDF(const Vector3f& wo, const Vector3f& wi) const override;
	virtual Color BTDF(const Vector3f& wt, const Vector3f& wi) const override;
    virtual bool SampleWt(const Vector3f& wo, Vector3f& wt) const override;
	virtual Vector3f SampleWi(const Vector3f& wo, float& pdf) const override;

private:
	static float Fresnel(float eta_i, float eta_t, float cos_i, float cos_t);
	float mEta;
	Color mTransmissionColor;
};