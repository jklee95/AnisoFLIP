#include "Anisotropic.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;

Anisotropic::Anisotropic(GridData& INDEX)
	:Interpolation(INDEX)
{
}

Anisotropic::~Anisotropic()
{
}

void Anisotropic::particleToGrid(XMFLOAT2 particlePos, XMFLOAT2 particleVel,
	float dt, vector<XMFLOAT2>& gridPos, vector<STATE>& gridState)
{
	XMFLOAT2 pos = particlePos;

	XMINT2 minIndex = _computeIndexRange(pos, VALUE::MIN, 4.0f);
	XMINT2 maxIndex = _computeIndexRange(pos, VALUE::MAX, 4.0f);

	float sum = 0.0f;


	XMFLOAT4X4 invRotate, invScale;
	_computeInverseRotScale(&invRotate, &invScale, particleVel * dt);

	XMFLOAT2 partTransPos = pos * invRotate * invScale;


	for (int i = minIndex.x; i <= maxIndex.x; i++)
	{
		for (int j = minIndex.y; j <= maxIndex.y; j++)
		{
			XMFLOAT2 gridTransPos = gridPos[_INDEX(i, j)] * invRotate * invScale;

			XMFLOAT2 dist = f2_fabsf(partTransPos - gridTransPos);

			float weightX = _cubicSpline(dist.x);
			float weightY = _cubicSpline(dist.y);
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

XMFLOAT2 Anisotropic::gridToParticle(vector<XMFLOAT2>& oldVel, XMFLOAT2 particlePos,
	XMFLOAT2 vel_dt, vector<XMFLOAT2>& gridPos, vector<STATE>& gridState)
{
	XMFLOAT2 pos = particlePos;

	XMINT2 minIndex = _computeIndexRange(pos, VALUE::MIN, 4.0f);
	XMINT2 maxIndex = _computeIndexRange(pos, VALUE::MAX, 4.0f);

	float sum = 0.0f;


	XMFLOAT4X4 invRotate, invScale;
	_computeInverseRotScale(&invRotate, &invScale, vel_dt);

	XMFLOAT2 partTransPos = pos * invRotate * invScale;


	for (int i = minIndex.x; i <= maxIndex.x; i++)
	{
		for (int j = minIndex.y; j <= maxIndex.y; j++)
		{
			XMFLOAT2 gridTransPos = gridPos[_INDEX(i, j)] * invRotate * invScale;

			XMFLOAT2 dist = f2_fabsf(partTransPos - gridTransPos);

			float weightX = _cubicSpline(dist.x);
			float weightY = _cubicSpline(dist.y);
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

void Anisotropic::_computeInverseRotScale(DirectX::XMFLOAT4X4* rotM, DirectX::XMFLOAT4X4* scaleM, XMFLOAT2 vel)
{
	float vel_length = f2_length(vel);
	float k = (vel_length * 0.5f) * (vel_length * 0.5f);
	float a = min(sqrtf((k + sqrtf(k * k + 64)) / 2.0f), 4.0f);
	float b = 4.0f / a;

	*scaleM =
	{
		2.0f / a,      0.0f,  0.0f,  0.0f,
			0.0f,  2.0f / b,  0.0f,  0.0f,
			0.0f,      0.0f,  1.0f,  0.0f,
			0.0f,      0.0f,  0.0f,  1.0f
	};

	float cosX1 = vel_length > FLT_EPSILON ? vel / vel_length * XMFLOAT2(1.0f, 0.0f) : 1.0f;
	float sinX1 = vel_length > FLT_EPSILON ? vel / vel_length * XMFLOAT2(0.0f, 1.0f) : 0.0f;

	*rotM =
	{
		 cosX1, -sinX1,  0.0f,  0.0f,
		 sinX1,  cosX1,  0.0f,  0.0f,
		 0.0f,    0.0f,  1.0f,  0.0f,
		 0.0f,    0.0f,  0.0f,  1.0f
	};
}