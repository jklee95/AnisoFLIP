#pragma once
#include "GridLiquid.h"
#include "Interpolation.h"
#include "Linear.h"

class PICFLIP : public GridLiquid
{
public:
	PICFLIP(int x, int y, EX ex, float timeStep);
	~PICFLIP() override;

	void setFlipRatio(int value);

private:
	Interpolation* _interp;

	std::vector<DirectX::XMFLOAT2> _oldVel;

	float _flipRatio = 0.0f;

	void _initialize(EX ex) override;

	void _update() override;
	void _advect();
	void _force();
	void _project();

	void _updateParticlePos();
};

