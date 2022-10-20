#pragma once
#include "Interpolation.h"

// row major
inline DirectX::XMFLOAT2 operator*(DirectX::XMFLOAT2 vec, DirectX::XMFLOAT4X4 mat)
{
	// XMFLOAT4(vec.x, vec.y, 0.0f, 1.0f)
	return
	{
		(vec.x * mat._11) + (vec.y * mat._21) + (0.0f * mat._31) + (1.0f * mat._41),
		(vec.x * mat._12) + (vec.y * mat._22) + (0.0f * mat._32) + (1.0f * mat._42)
	};
}

class Anisotropic : public Interpolation
{
public:
	Anisotropic(GridData INDEX);
	~Anisotropic();

	void particleToGrid(
		DirectX::XMFLOAT2 particlePos, DirectX::XMFLOAT2 particleVel,
		float dt,
		std::vector<DirectX::XMFLOAT2>& gridPos,
		std::vector<STATE>& gridState) override;

	DirectX::XMFLOAT2 gridToParticle(
		std::vector<DirectX::XMFLOAT2>& oldvel,
		DirectX::XMFLOAT2 particlePos, DirectX::XMFLOAT2 vel_dt,
		std::vector<DirectX::XMFLOAT2>& gridPos,
		std::vector<STATE>& gridState) override;

private:
	void _computeInverseRotScale(
		DirectX::XMFLOAT4X4* rotM, DirectX::XMFLOAT4X4* scaleM, 
		DirectX::XMFLOAT2 vel);

	inline float f2_length(DirectX::XMFLOAT2 f1)
	{
		return sqrtf(f1.x * f1.x + f1.y * f1.y);
	}

};

