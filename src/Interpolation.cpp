#include "Interpolation.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;

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