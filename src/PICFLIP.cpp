#include "PICFLIP.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;
using namespace DXViewer::xmint2;

PICFLIP::PICFLIP(int x, int y, EX ex, float timeStep)
	:GridLiquid(x, y, timeStep)
{
	_initialize(ex);

	_interp = new Linear(_INDEX);
}

PICFLIP::~PICFLIP()
{
	delete _interp;
}

void PICFLIP::setFlipRatio(int value)
{
	_flipRatio = static_cast<float>(value) / 100.0f;
}

void PICFLIP::_initialize(EX ex)
{
	GridLiquid::_initialize(ex);

	size_t vSize = static_cast<size_t>(_gridCount.x) * static_cast<size_t>(_gridCount.y);
	_oldVel.assign(vSize, { 0.0f, 0.0f });
}



void PICFLIP::_update()
{
	_advect();

	_force();
	_setBoundary(_gridVelocity);
	_setFreeSurface(_gridVelocity);

	_project();
	// Solve boundary condition again due to numerical errors in previous step.
	_setBoundary(_gridVelocity);
	_updateParticlePos();

	_paintLiquid();

	
}

void PICFLIP::_advect()
{
	XMINT2 N = _gridCount - 2;

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		_interp->particleToGrid(_particlePosition[i], _particleVelocity[i], _timeStep, _gridPosition, _gridState);
	}

	for (int j = 0; j < _gridCount.y; j++)
	{
		for (int i = 0; i < _gridCount.x; i++)
		{
			_interp->setGridVelocity(_gridVelocity, _oldVel, i, j);
		}
	}
}

void PICFLIP::_force()
{
	float dt = _timeStep;
	XMINT2 N = _gridCount - 2;

	for (int j = 1; j <= N.y; j++)
	{
		for (int i = 1; i <= N.x; i++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				// Gravity
				_gridVelocity[_INDEX(i, j)].y -= 9.8f * dt;
			}

		}
	}
}


void PICFLIP::_project()
{
	XMINT2 N = _gridCount - 2;

	// Initialize the divergence and pressure.
	for (int j = 1; j <= N.y; j++)
	{
		for (int i = 1; i <= N.x; i++)
		{
			_gridDivergence[_INDEX(i, j)] =
				0.5f * (_gridVelocity[_INDEX(i + 1, j)].x - _gridVelocity[_INDEX(i - 1, j)].x
					+ _gridVelocity[_INDEX(i, j + 1)].y - _gridVelocity[_INDEX(i, j - 1)].y);

			_gridPressure[_INDEX(i, j)] = 0.0f;

		}
	}

	_setBoundary(_gridDivergence);
	_setBoundary(_gridPressure);
	
	// Gauss-Seidel method
	for (int iter = 0; iter < 200; iter++)
	{
		
		for (int j = 1; j <= N.y; j++)
		{
			for (int i = 1; i <= N.x; i++)
			{

				if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
				{
					_gridPressure[_INDEX(i, j)] =
						(
							_gridDivergence[_INDEX(i, j)] -
							(_gridPressure[_INDEX(i + 1, j)] + _gridPressure[_INDEX(i - 1, j)] +
								_gridPressure[_INDEX(i, j + 1)] + _gridPressure[_INDEX(i, j - 1)])
							) / -4.0f;

				}
			
			}

		}
		_setBoundary(_gridPressure);
	}

	for (int j = 1; j <= N.y; j++)
	{
		for (int i = 1; i <= N.x; i++)
		{
			if (_gridState[_INDEX(i, j)] == STATE::LIQUID)
			{
				// Apply the pressure force to the velocity.
				_gridVelocity[_INDEX(i, j)].x -= (_gridPressure[_INDEX(i + 1, j)] - _gridPressure[_INDEX(i - 1, j)]) * 0.5f;
				_gridVelocity[_INDEX(i, j)].y -= (_gridPressure[_INDEX(i, j + 1)] - _gridPressure[_INDEX(i, j - 1)]) * 0.5f;

			}
		}
	}

}

void PICFLIP::_updateParticlePos()
{
	float dt = _timeStep;
	XMINT2 N = _gridCount - 2;

	for (int i = 0; i < _oldVel.size(); i++)
	{
		_oldVel[i] = _gridVelocity[i] - _oldVel[i];
	}

	float yMax = _gridPosition[_INDEX(0, N.y + 1)].y - 0.5f;
	float yMin = _gridPosition[_INDEX(0, 0)].y + 0.5f;
	float xMax = _gridPosition[_INDEX(N.x + 1, 0)].x - 0.5f;
	float xMin = _gridPosition[_INDEX(0, 0)].x + 0.5f;

	for (int i = 0; i < _particlePosition.size(); i++)
	{
		XMFLOAT2 _picVel =			// Current Velocity
			_interp->gridToParticle(_gridVelocity, 
				_particlePosition[i], (_particleVelocity[i] * dt),
				_gridPosition, _gridState);
		XMFLOAT2 _flipVel = 
			_particleVelocity[i]	// Previous Velocity
			+ _interp->gridToParticle(_oldVel,
				_particlePosition[i], (_particleVelocity[i] * dt), 
				_gridPosition, _gridState);

		// PIC/FLIP blending
		_particleVelocity[i] = _picVel * (1 - _flipRatio) + _flipVel * _flipRatio;
		_particlePosition[i] += _particleVelocity[i] * dt;

		// Boundary condition
		if (_particlePosition[i].x > xMax) _particlePosition[i].x = xMax;
		else if (_particlePosition[i].x < xMin) _particlePosition[i].x = xMin;

		if (_particlePosition[i].y > yMax) _particlePosition[i].y = yMax;
		else if (_particlePosition[i].y < yMin) _particlePosition[i].y = yMin;
	}
}
