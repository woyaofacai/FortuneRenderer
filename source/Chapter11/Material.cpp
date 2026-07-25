#include "Material.h"

Color LambertMaterial::BRDF(const Vector3f& wo, const Vector3f& wi) const
{
	return mAlbedo * INV_PI;
}

Color ConductorSpecularMaterial::BRDF(const Vector3f& wo, const Vector3f& wi) const
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

Color DielectricSpecularMaterial::BRDF(const Vector3f& wo, const Vector3f& wi) const
{
	float cos_i = std::fabs(wi.z);
	float eta_i, eta_t;
	if (wi.z > 0)
	{
		eta_i = 1.0f;
		eta_t = mEta;
	}
	else
	{
		eta_i = mEta;
		eta_t = 1.0f;
	}

	Vector3f wt;
	float Fr = 1.0f;
	if (ComputeRefractVector(wi, eta_i, eta_t, wt))
	{
        float cos_t = std::fabs(wt.z);
        Fr = Fresnel(eta_i, eta_t, cos_i, cos_t);
	}
	return Color(Fr) / std::max(cos_i, 1e-3f);
}

Color DielectricSpecularMaterial::BTDF(const Vector3f& wt, const Vector3f& wi) const
{
	float cos_i = std::fabs(wi.z);
	float eta_i, eta_t;
	if (wi.z > 0)
	{
		eta_i = 1.0f;
		eta_t = mEta;
	}
	else
	{
		eta_i = mEta;
		eta_t = 1.0f;
	}

	float cos_t = std::fabs(wt.z);
	float Fr = Fresnel(eta_i, eta_t, cos_i, cos_t);
    return mTransmissionColor * glm::max(0.0f, 1.0f - Fr) * eta_t * eta_t / (eta_i * eta_i * std::max(cos_i, 1e-3f));
}

float DielectricSpecularMaterial::Fresnel(float eta_i, float eta_t, float cos_i, float cos_t)
{
	float r1 = (eta_t * cos_i - eta_i * cos_t) / glm::max(1e-4f, eta_t * cos_i + eta_i * cos_t);
	float r2 = (eta_i * cos_i - eta_t * cos_t) / glm::max(1e-4f, eta_i * cos_i + eta_t * cos_t);
	return 0.5f * (r1 * r1 + r2 * r2);
}

bool DielectricSpecularMaterial::SampleWt(const Vector3f& wi, Vector3f& wt) const
{
	float cos_i = std::fabs(wi.z);
	float eta_i, eta_t;
	if (wi.z > 0)
	{
		eta_i = 1.0f;
		eta_t = mEta;
	}
	else
	{
		eta_i = mEta;
		eta_t = 1.0f;
	}

	return ComputeRefractVector(wi, eta_i, eta_t, wt);
}
