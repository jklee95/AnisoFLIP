#pragma once
#include "gridsimheader.h"

class Interpolation
{
public:
	Interpolation(GridData INDEX);
	virtual ~Interpolation();

	virtual void particleToGrid(
		DirectX::XMFLOAT2 particlePos, DirectX::XMFLOAT2 particleVel,
		float dt,
		std::vector<DirectX::XMFLOAT2>& gridPos,
		std::vector<STATE>& gridState) = 0;

	virtual DirectX::XMFLOAT2 gridToParticle(
		std::vector<DirectX::XMFLOAT2>& oldvel,
		DirectX::XMFLOAT2 particlePos, DirectX::XMFLOAT2 vel_dt,
		std::vector<DirectX::XMFLOAT2>& gridPos,
		std::vector<STATE>& gridState) = 0;

	void setGridVelocity(
		std::vector<DirectX::XMFLOAT2>& gridVel,
		std::vector<DirectX::XMFLOAT2>& oldVel,
		int i, int j);

protected:
	GridData _INDEX;
	std::vector<DirectX::XMFLOAT2> _tempVel;
	std::vector<float> _pCount;
	std::vector<float> _weights;

	DirectX::XMINT2 _computeIndexRange(DirectX::XMFLOAT2 pos, VALUE vState, float amount);
	DirectX::XMINT2 _computeCenterMinMaxIndex(VALUE vState, DirectX::XMFLOAT2 particlePos);

	inline DirectX::XMFLOAT2 f2_fabsf(DirectX::XMFLOAT2 f1)
	{
		return DirectX::XMFLOAT2(fabsf(f1.x), fabsf(f1.y));
	}
};

