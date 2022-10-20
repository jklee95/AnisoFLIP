#include "Linear.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;

Linear::Linear(GridData INDEX)
	:Interpolation(INDEX)
{
}

Linear::~Linear()
{
}


void Linear::particleToGrid(XMFLOAT2 particlePos, XMFLOAT2 particleVel,
	float dt, vector<XMFLOAT2>& gridPos, vector<STATE>& gridState)
{
	XMFLOAT2 pos = particlePos;
	XMINT2 minIndex = _computeIndexRange(pos, VALUE::MIN, 2.0f);
	XMINT2 maxIndex = _computeIndexRange(pos, VALUE::MAX, 2.0f);

	float sum = 0.0f;

	for (int i = minIndex.x; i <= maxIndex.x; i++)
	{
		for (int j = minIndex.y; j <= maxIndex.y; j++)
		{
			XMFLOAT2 dist = f2_fabsf(pos - gridPos[_INDEX(i, j)]);

			float weightX = _linearSpline(dist.x);
			float weightY = _linearSpline(dist.y);
			float weight = gridState[_INDEX(i, j)] == STATE::LIQUID ? weightX * weightY : 0.0f;

			_weights[_INDEX(i, j)] = weight;

			sum += weight;
		}
	}


	for (int i = minIndex.x; i <= maxIndex.x; i++)
	{
		for (int j = minIndex.y; j <= maxIndex.y; j++)
		{

			_weights[_INDEX(i, j)] /= sum;

			_pCount[_INDEX(i, j)] += _weights[_INDEX(i, j)];
			_tempVel[_INDEX(i, j)] += particleVel * _weights[_INDEX(i, j)];

			// reset
			_weights[_INDEX(i, j)] = 0.0f;
		}
	}
}

XMFLOAT2 Linear::gridToParticle(vector<XMFLOAT2>& oldVel, XMFLOAT2 particlePos, 
	XMFLOAT2 vel_dt, vector<XMFLOAT2>& gridPos, vector<STATE>& gridState)
{
	XMFLOAT2 pos = particlePos;
	XMINT2 minIndex = _computeIndexRange(pos, VALUE::MIN, 2.0f);
	XMINT2 maxIndex = _computeIndexRange(pos, VALUE::MAX, 2.0f);

	float sum = 0.0f;

	for (int i = minIndex.x; i <= maxIndex.x; i++)
	{
		for (int j = minIndex.y; j <= maxIndex.y; j++)
		{
			XMFLOAT2 dist = f2_fabsf(pos - gridPos[_INDEX(i, j)]);

			float weightX = _linearSpline(dist.x);
			float weightY = _linearSpline(dist.y);
			float weight = gridState[_INDEX(i, j)] == STATE::LIQUID ? weightX * weightY : 0.0f;

			_weights[_INDEX(i, j)] = weight;

			sum += weight;
		}
	}

	XMFLOAT2 result = { 0.0f , 0.0f };

	
	for (int i = minIndex.x; i <= maxIndex.x; i++)
	{
		for (int j = minIndex.y; j <= maxIndex.y; j++)
		{
			_weights[_INDEX(i, j)] /= sum;

			XMFLOAT2 vel = oldVel[_INDEX(i, j)];
			result += vel * _weights[_INDEX(i, j)];

			// reset
			_weights[_INDEX(i, j)] = 0.0f;
		}
	}

	return result;
}

float Linear::_linearSpline(float x)
{
	if (x >= 0.0f && x < 2.0f)
		return 2.0f - x;
	else
		return 0.0f;
}