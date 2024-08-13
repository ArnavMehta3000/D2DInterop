#include "Window.h"

void Window::Run()
{
	Create(L"App", WS_OVERLAPPEDWINDOW, 0, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720);
	ShowWindow(m_hWnd, SW_SHOWNORMAL);

	Init();

	m_isRunning = true;

	MSG msg = { };
	while (m_isRunning)
	{
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				m_isRunning = false;
			}
		}

		Update();
		Render();
	}
}

Window::~Window()
{
	Shutdown();
}

void Window::Init()
{
	m_d3d = std::make_unique<DX::DeviceResources>(
		DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT_D32_FLOAT, 2,
		D3D_FEATURE_LEVEL_11_1
	);
	m_d3d->SetWindow(m_hWnd, 1280, 720);
	m_d3d->CreateDeviceResources();
	m_d3d->CreateWindowSizeDependentResources();

	m_shape = GeometricPrimitive::CreateSphere(m_d3d->GetD3DDeviceContext());
	m_world = Matrix::Identity;

	auto size = m_d3d->GetOutputSize();
	m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
		Vector3::Zero, Vector3::UnitY);
	m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		float(size.right) / float(size.bottom), 0.1f, 10.f);

	HRESULT hr{ S_OK };
	hr = m_d3d->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&m_dxgiSurface));

	D2D1_FACTORY_OPTIONS opts{};
	opts.debugLevel = D2D1_DEBUG_LEVEL_NONE;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &opts, &m_d2dFactory);

	float dpi = ::GetDpiForWindow(m_hWnd);
	
	D2D1_RENDER_TARGET_PROPERTIES props = 
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpi, dpi);

	hr = m_d2dFactory->CreateDxgiSurfaceRenderTarget(m_dxgiSurface.Get(), &props, &m_d2dRenderTarget);


}

void Window::Update()
{
	int x = 0;
}

float t = 0.0f;
void Window::Render()
{
	t += 0.001f;
	// Begin frame
	auto context      = m_d3d->GetD3DDeviceContext();
	auto renderTarget = m_d3d->GetRenderTargetView();
	auto depthStencil = m_d3d->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_d3d->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_shape->Draw(m_world, m_view, m_proj);

	HRESULT hr{ S_OK };
	DXGI_SWAP_CHAIN_DESC desc;
	hr = m_d3d->GetSwapChain()->GetDesc(&desc);

	if (SUCCEEDED(hr) && m_d2dRenderTarget)
	{
		D2D1_SIZE_F targetSize = m_d2dRenderTarget->GetSize();
		ComPtr<ID2D1SolidColorBrush> brush;
		D2D1_COLOR_F color{ 1.0f, 0.0f, 0.0f, 1.0f };
		hr = m_d2dRenderTarget->CreateSolidColorBrush(color, &brush);
		
		m_d2dRenderTarget->BeginDraw();

		brush->SetTransform(D2D1::Matrix3x2F::Scale(targetSize));
		D2D1_RECT_F rect = D2D1::RectF(
			100.0f,
			100.0f,
			std::abs(std::sinf(t) * 500.0f),
			std::abs(std::cosf(t) * 500.0f));

		D2D1_ELLIPSE ellipse = D2D1::Ellipse({ 500.0f, 500.0f }, std::abs(std::sinf(t) * 500.0f), std::abs(std::sinf(t) * 500.0f));

		m_d2dRenderTarget->DrawRectangle(&rect, brush.Get(), 5.0f);
		m_d2dRenderTarget->DrawEllipse(ellipse, brush.Get(), 7.0f);

		m_d2dRenderTarget->EndDraw();
	}

	m_d3d->Present();
}

void Window::Shutdown()
{
	m_shape.reset();
	m_d3d.reset();
}

PCWSTR Window::ClassName() const
{
	return L"WndClass";
}

LRESULT Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		m_isRunning = false;
		return 0;

	case WM_SIZE:
		if (m_d3d)
		{
			m_d3d->WindowSizeChanged(LOWORD(lParam), HIWORD(lParam));

			auto size = m_d3d->GetOutputSize();
			m_view = Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f), Vector3::Zero, Vector3::UnitY);
			m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f, float(size.right) / float(size.bottom), 0.1f, 10.f);
			break;
		}
	}

	return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}
