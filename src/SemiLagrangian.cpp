#include "SemiLagrangian.h"
using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;

SemiLagrangian::SemiLagrangian(GridData& INDEX)
	:Interpolation(INDEX)
{
}

SemiLagrangian::~SemiLagrangian()
{
}

void SemiLagrangian::particleToGrid(XMFLOAT2 particlePos, XMFLOAT2 particleVel,
	float dt, vector<XMFLOAT2>& gridPos, vector<STATE>& gridState)
{
}

XMFLOAT2 SemiLagrangian::gridToParticle(vector<XMFLOAT2>& oldVel, XMFLOAT2 particlePos,
	XMFLOAT2 vel_dt, vector<XMFLOAT2>& gridPos, vector<STATE>& gridState)
{
	XMFLOAT2 pos = particlePos;

	XMINT2 minIndex = _computeCenterMinMaxIndex(VALUE::MIN, pos);
	XMINT2 maxIndex = _computeCenterMinMaxIndex(VALUE::MAX, pos);

	// Ratio of the Distance.
	XMFLOAT2 ratio = (pos - gridPos[_INDEX(minIndex.x, minIndex.y)]);

	// Since the grid spacing is 1 (i.e. the difference in distance is between 0 and 1), 
	// Normalization of the difference is not necessary.
	float minMinRatio = gridState[_INDEX(minIndex.x, minIndex.y)] == STATE::LIQUID ? (1.0f - ratio.x) * (1.0f - ratio.y) : 0.0f;
	float minMaxRatio = gridState[_INDEX(minIndex.x, maxIndex.y)] == STATE::LIQUID ? (1.0f - ratio.x) * ratio.y : 0.0f;
	float maxMinRatio = gridState[_INDEX(maxIndex.x, minIndex.y)] == STATE::LIQUID ? ratio.x * (1.0f - ratio.y) : 0.0f;
	float maxMaxRatio = gridState[_INDEX(maxIndex.x, maxIndex.y)] == STATE::LIQUID ? ratio.x * ratio.y : 0.0f;

	// Normalization
	float totalRatio = minMinRatio + minMaxRatio + maxMinRatio + maxMaxRatio;
	if (totalRatio > FLT_EPSILON)
	{
		minMinRatio /= totalRatio;
		minMaxRatio /= totalRatio;
		maxMinRatio /= totalRatio;
		maxMaxRatio /= totalRatio;
	}

	// Velocity of 4 corners.
	XMFLOAT2 minMinVel = oldVel[_INDEX(minIndex.x, minIndex.y)];
	XMFLOAT2 minMaxVel = oldVel[_INDEX(minIndex.x, maxIndex.y)];
	XMFLOAT2 maxMinVel = oldVel[_INDEX(maxIndex.x, minIndex.y)];
	XMFLOAT2 maxMaxVel = oldVel[_INDEX(maxIndex.x, maxIndex.y)];

	return
		// Bilinear interpolation
		minMinVel * minMinRatio + minMaxVel * minMaxRatio
		+ maxMinVel * maxMinRatio + maxMaxVel * maxMaxRatio;
}