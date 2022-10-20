#include "Interpolation.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;
using namespace DXViewer::xmint2;

Interpolation::Interpolation(GridData INDEX)
	:_INDEX(INDEX)
{
}

Interpolation::~Interpolation()
{
}

void Interpolation::setGridVelocity(vector<XMFLOAT2>& gridVel, vector<XMFLOAT2>& oldVel, int i, int j)
{
	if (_pCount[_INDEX(i, j)] > FLT_EPSILON)
	{
		// Average
		gridVel[_INDEX(i, j)] = oldVel[_INDEX(i, j)] = _tempVel[_INDEX(i, j)] / (_pCount[_INDEX(i, j)]);
	}
	else
	{
		gridVel[_INDEX(i, j)] = oldVel[_INDEX(i, j)] = { 0.0f, 0.0f };
	}

	// Reset
	_tempVel[_INDEX(i, j)] = { 0.0f, 0.0f };
	_pCount[_INDEX(i, j)] = 0.0f;
}

XMINT2 Interpolation::_computeIndexRange(XMFLOAT2 pos, VALUE vState, float amount)
{
	XMINT2 N = _INDEX.gridCount - 2;

	switch (vState)
	{
		case VALUE::MIN:
			return { max(static_cast<int>(ceil(pos.x - amount)), 1),  max(static_cast<int>(ceil(pos.y - amount)), 1) };
			break;

		case VALUE::MAX:
			return { min(static_cast<int>(floor(pos.x + amount)), N.x), min(static_cast<int>(floor(pos.y + amount)), N.y) };
			break;

		default:
			return { 0, 0 };
	}
}

// Center-type velocity storage
XMINT2 Interpolation::_computeCenterMinMaxIndex(VALUE vState, XMFLOAT2 particlePos)
{
	switch (vState)
	{
		case VALUE::MIN:
			return { static_cast<int>(floor(particlePos.x)), static_cast<int>(floor(particlePos.y)) };
			break;
		case VALUE::MAX:
			return { static_cast<int>(ceil(particlePos.x)), static_cast<int>(ceil(particlePos.y)) };
			break;
		default:
			return { -1, -1 };
			break;
	}
}