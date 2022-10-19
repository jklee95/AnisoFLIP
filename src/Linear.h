#pragma once
#include "Interpolation.h"

class Linear : public Interpolation
{
public:
	Linear(GridData INDEX);
	~Linear();

	void particleToGrid(
		DirectX::XMFLOAT2 particlePos, DirectX::XMFLOAT2 particleVel,
		float dt,
		std::vector<DirectX::XMFLOAT2>& gridPos,
		std::vector<STATE>& gridState) override;

private:
	float _linearSpline(float x);
};

