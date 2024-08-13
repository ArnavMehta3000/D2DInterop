#include "BaseWindow.h"
#include "DeviceResources.h"
#include <SimpleMath.h>
#include <GeometricPrimitive.h>
#include <memory>
#include <DirectXColors.h>
#include <d2d1_3helper.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "d2d1.lib")

class Window : public BaseWindow<Window>
{
public:
	~Window();
	void Init();
	void Run();
	void Update();
	void Render();
	void Shutdown();

	// Inherited via BaseWindow
	PCWSTR ClassName() const override;
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
	std::unique_ptr<DX::DeviceResources> m_d3d;
	std::unique_ptr<GeometricPrimitive> m_shape;
	ComPtr<IDXGISurface1> m_dxgiSurface;
	ComPtr<ID2D1Factory1> m_d2dFactory;
	ComPtr<ID2D1RenderTarget> m_d2dRenderTarget;
	bool m_isRunning;

	Matrix m_world;
	Matrix m_view;
	Matrix m_proj;
};