#include "Linear.h"
using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;

Linear::Linear(GridData INDEX)
	:Interpolation(INDEX)
{
	size_t vSize = static_cast<size_t>(INDEX.gridCount.x) * static_cast<size_t>(INDEX.gridCount.y);
	_tempVel.assign(vSize, { 0.0f, 0.0f });
	_pCount.assign(vSize, 0.0f);
}

Linear::~Linear()
{
}


void Linear::particleToGrid(XMFLOAT2 particlePos, XMFLOAT2 particleVel,
	float dt, vector<XMFLOAT2>& gridPos, vector<STATE>& gridState)
{
	XMFLOAT2 pos = particlePos;
	XMFLOAT2 vel = particleVel;

	XMINT2 minIndex = _computeCenterMinMaxIndex(VALUE::MIN, pos);
	XMINT2 maxIndex = _computeCenterMinMaxIndex(VALUE::MAX, pos);

	// Ratio of the Distance.
	XMFLOAT2 ratio = pos - gridPos[_INDEX(minIndex.x, minIndex.y)];

	// Since the grid spacing is 1 (i.e. the difference in distance is between 0 and 1), 
	// Normalization of the difference is not necessary.
	float minMinRatio = gridState[_INDEX(minIndex.x, minIndex.y)] == STATE::LIQUID ? (1.0f - ratio.x) * (1.0f - ratio.y) : 0.0f;
	float minMaxRatio = gridState[_INDEX(minIndex.x, maxIndex.y)] == STATE::LIQUID ? (1.0f - ratio.x) * ratio.y : 0.0f;
	float maxMinRatio = gridState[_INDEX(maxIndex.x, minIndex.y)] == STATE::LIQUID ? ratio.x * (1.0f - ratio.y) : 0.0f;
	float maxMaxRatio = gridState[_INDEX(maxIndex.x, maxIndex.y)] == STATE::LIQUID ? ratio.x * ratio.y : 0.0f;

	// Normalization of the ratio.
	float totalRatio = minMinRatio + minMaxRatio + maxMinRatio + maxMaxRatio;
	if (totalRatio > FLT_EPSILON)
	{
		minMinRatio /= totalRatio;
		minMaxRatio /= totalRatio;
		maxMinRatio /= totalRatio;
		maxMaxRatio /= totalRatio;
	}

	// Count the number of particles affecting _INDEX(i, j).
	_pCount[_INDEX(minIndex.x, minIndex.y)] += minMinRatio;
	_pCount[_INDEX(minIndex.x, maxIndex.y)] += minMaxRatio;
	_pCount[_INDEX(maxIndex.x, minIndex.y)] += maxMinRatio;
	_pCount[_INDEX(maxIndex.x, maxIndex.y)] += maxMaxRatio;

	// Add the velocity multiplied by the ratio.
	_tempVel[_INDEX(minIndex.x, minIndex.y)] += vel * minMinRatio;
	_tempVel[_INDEX(minIndex.x, maxIndex.y)] += vel * minMaxRatio;
	_tempVel[_INDEX(maxIndex.x, minIndex.y)] += vel * maxMinRatio;
	_tempVel[_INDEX(maxIndex.x, maxIndex.y)] += vel * maxMaxRatio;
}

XMFLOAT2 Linear::gridToParticle(vector<XMFLOAT2>& oldVel, XMFLOAT2 particlePos, 
	XMFLOAT2 vel_dt, vector<XMFLOAT2>& gridPos, vector<STATE>& gridState)
{
	XMFLOAT2 pos = particlePos;

	XMINT2 minIndex = _computeCenterMinMaxIndex(VALUE::MIN, pos);
	XMINT2 maxIndex = _computeCenterMinMaxIndex(VALUE::MAX, pos);

	XMFLOAT2 ratio = (pos - gridPos[_INDEX(minIndex.x, minIndex.y)]);

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

	XMFLOAT2 minMinVel = oldVel[_INDEX(minIndex.x, minIndex.y)];
	XMFLOAT2 minMaxVel = oldVel[_INDEX(minIndex.x, maxIndex.y)];
	XMFLOAT2 maxMinVel = oldVel[_INDEX(maxIndex.x, minIndex.y)];
	XMFLOAT2 maxMaxVel = oldVel[_INDEX(maxIndex.x, maxIndex.y)];

	return
		minMinVel * minMinRatio + minMaxVel * minMaxRatio
		+ maxMinVel * maxMinRatio + maxMaxVel * maxMaxRatio;
}

float Linear::_linearSpline(float x)
{
	if (x >= 0.0f && x < 2.0f)
		return 2.0f - x;
	else
		return 0.0f;
}