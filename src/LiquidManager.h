#pragma once
#include <ctime>

#include "Win32App.h" // This includes ISimulation.h.
					  // Win32App is required in main().

// These include GridLiquid.h.
#include "Eulerian.h"
#include "PICFLIP.h"
#include "Interpolation.h"
#include "SemiLagrangian.h"
#include "Linear.h"
#include "CubicSpline.h"
#include "Anisotropic.h"

class LiquidManager : public ISimulation
{
public:
	LiquidManager(int x, int y, float timeStep);
	~LiquidManager() override;


#pragma region Implementation
	// ################################## Implementation ####################################
	// Simulation methods
	void iUpdate() override;
	void iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer) override;

	// Mesh methods
	std::vector<Vertex>& iGetVertices() override;
	std::vector<unsigned int>& iGetIndices() override;
	UINT iGetVertexBufferSize() override;
	UINT iGetIndexBufferSize() override;

	// DirectX methods
	void iCreateObject(std::vector<ConstantBuffer>& constantBuffer) override;
	void iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i) override;
	void iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i) override;
	void iSetDXApp(DX12App* dxApp) override;
	UINT iGetConstantBufferSize() override;
	DirectX::XMINT3 iGetObjectCount() override;
	DirectX::XMFLOAT3 iGetObjectSize() override;
	DirectX::XMFLOAT3 iGetObjectPositionOffset() override;
	bool iIsUpdated() override;

	// WndProc methods
	void iWMCreate(HWND hwnd, HINSTANCE hInstance) override;
	void iWMCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance) override;
	void iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance) override;
	void iWMTimer(HWND hwnd) override;
	void iWMDestory(HWND hwnd) override;
	// #######################################################################################
#pragma endregion


private:
	enum class COM
	{
		GRID_BTN, PARTICLE_BTN, VELOCITY_BTN,
		PLAY, STOP, NEXTSTEP,
		EX_GROUP, DAM_RADIO, DROP_RADIO,
		SOLVER_GROUP, EULERIAN_RADIO, PICFLIP_RADIO,
		RATIO_BAR, PIC_TEXT, PIC_RATIO, FLIP_TEXT, FLIP_RATIO,
		INTERP_GROUP, LINEAR_RADIO, CUBIC_RADIO, ANISO_RADIO,
		TIME_TEXT, FRAME_TEXT
	};

	enum class SIM
	{
		EULERIAN, FLIP
	};

	enum class INTERP
	{
		SEMI_LAGRANGIAN, LINEAR, CUBIC, ANISO
	};


	std::vector<GridLiquid*> _sim;
	std::vector<Interpolation*> _interp;

	// 0 : Eulerian
	// 1 : PIC/FLIP
	int _simIndex;

	// 0 : Linear
	// 1 : Cubic
	// 2 : Aniso

	int _scrollPos = 99;

	DX12App* _dxapp = nullptr;
					// grid, particle, velocity
	bool _drawFlag[3] = { true, true, false };
	bool _updateFlag = true;

	clock_t _simTime = 0;
	int _simFrame = 0;

	EX _ex = EX::DROP;
	GridData _index;

	void _setDrawFlag(FLAG flagType, bool flag);
	bool _getDrawFlag(FLAG flagType);

	void _setSimInterp(int interpIndex);
};

