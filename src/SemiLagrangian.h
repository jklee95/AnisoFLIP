#pragma once
#include "Interpolation.h"

class SemiLagrangian : public Interpolation
{
public:
	SemiLagrangian(GridData& INDEX);
	~SemiLagrangian();

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

};

