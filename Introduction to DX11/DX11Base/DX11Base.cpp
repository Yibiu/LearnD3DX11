#include "DX11Base.h"


CDX11Base *g_d3dapp_ptr = NULL;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_d3dapp_ptr->msg_proc(hwnd, msg, wParam, lParam);
}


// CDX11Base
CDX11Base::CDX11Base()
{
	g_d3dapp_ptr = this;

	_inst = NULL;
	_wnd = NULL;
	_win_width = WINDOW_WIDTH;
	_win_height = WINDOW_HEIGHT;
	_enable4xMsaa = true;
	_m4xMsaa_quality = 0;
	_paused = false;
	_minimized = false;
	_maximized = false;
	_resizing = false;

	_device_ptr = NULL;
	_contex_ptr = NULL;
	_swap_chain_ptr = NULL;
	_render_target_view_ptr = NULL;
	_depth_stencil_view_ptr = NULL;
	_depth_stencil_buffer_ptr = NULL;
}

CDX11Base::~CDX11Base()
{
}

bool CDX11Base::init(HINSTANCE instance)
{
	bool success = false;

	do {
		if (!_init_window())
			break;
		if (!_init_d3d())
			break;

		success = true;
	} while (false);

	return success;
}

bool CDX11Base::on_resize()
{
	HRESULT hr = S_OK;

	do {
		if (NULL == _device_ptr || NULL == _contex_ptr || NULL == _swap_chain_ptr) {
			hr = E_FAIL;
			break;
		}
		if (NULL != _depth_stencil_buffer_ptr) {
			_depth_stencil_buffer_ptr->Release();
		}
		if (NULL != _render_target_view_ptr) {
			_render_target_view_ptr->Release();
		}
		if (NULL != _depth_stencil_view_ptr) {
			_depth_stencil_view_ptr->Release();
		}

		// Resize swap chain
		hr = _swap_chain_ptr->ResizeBuffers(1, _win_width, _win_height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		if (FAILED(hr))
			break;

		// Create target view
		ID3D11Texture2D *back_buffer_ptr;
		hr = _swap_chain_ptr->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer_ptr));
		if (FAILED(hr))
			break;
		hr = _device_ptr->CreateRenderTargetView(back_buffer_ptr, 0, &_render_target_view_ptr);
		back_buffer_ptr->Release();
		
		// Create the depth/stencil buffer and view
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = _win_width;
		desc.Height = _win_height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (_enable4xMsaa) {
			desc.SampleDesc.Count = 4;
			desc.SampleDesc.Quality = _m4xMsaa_quality - 1;
		}
		else {
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		hr = _device_ptr->CreateTexture2D(&desc, 0, &_depth_stencil_buffer_ptr);
		if (FAILED(hr))
			break;
		hr = _device_ptr->CreateDepthStencilView(_depth_stencil_buffer_ptr, 0, &_depth_stencil_view_ptr);
		if (FAILED(hr))
			break;
		
		// Bind the render target view and depth/stencil view to the pipeline
		_contex_ptr->OMSetRenderTargets(1, &_render_target_view_ptr, _depth_stencil_view_ptr);

		// Set the viewport transform
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(_win_width);
		viewport.Height = static_cast<float>(_win_height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		_contex_ptr->RSSetViewports(1, &viewport);
	} while (false);

	return (SUCCEEDED(hr));
}

void CDX11Base::update(float dt)
{

}

void CDX11Base::draw()
{

}

LRESULT CDX11Base::msg_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
		}
		return 0;
	case WM_SIZE:
		_win_width = LOWORD(lParam);
		_win_height = HIWORD(lParam);
		if (NULL != _device_ptr) {
			if (wParam == SIZE_MINIMIZED) {
				_minimized = true;
				_maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED) {
				_minimized = false;
				_maximized = true;
				on_resize();
			}
			else if (wParam == SIZE_RESTORED) {
				if (_minimized) {
					_minimized = false;
					on_resize();
				}
				else if (_maximized) {
					_maximized = false;
					on_resize();
				}
				else if (_resizing) {
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					on_resize();
				}
			}
		}
		return 0;
	case WM_ENTERSIZEMOVE:
		_resizing = true;
		return 0;
	case WM_EXITSIZEMOVE:
		_resizing = false;
		on_resize();
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		on_mouse_pressed(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		on_mouse_release(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		on_mouse_move(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void CDX11Base::on_mouse_pressed(WPARAM btnState, int x, int y)
{

}

void CDX11Base::on_mouse_release(WPARAM btnState, int x, int y)
{

}

void CDX11Base::on_mouse_move(WPARAM btnState, int x, int y)
{

}

int CDX11Base::run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			update(0);
			draw();
		}
	}

	return (int)msg.wParam;
}

HINSTANCE CDX11Base::get_inst() const
{
	return _inst;
}

HWND CDX11Base::get_wnd() const
{
	return _wnd;
}

float CDX11Base::get_ratio() const
{
	return static_cast<float>(_win_width) / _win_height;
}


bool CDX11Base::_init_window()
{
	// Registe window
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = _inst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";
	if (!RegisterClass(&wc))
		return false;

	// Create window
	RECT R = { 0, 0, _win_width, _win_height };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;
	_wnd = CreateWindow(L"D3DWndClassName", L"D3D11 Application",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, _inst, 0);
	if (NULL == _wnd)
		return false;
	ShowWindow(_wnd, SW_SHOW);
	UpdateWindow(_wnd);

	return true;
}

bool CDX11Base::_init_d3d()
{
	HRESULT hr = S_OK;

	do {
		// Create the device and device context
		UINT flags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL feature_level;
		hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, 0, 0, 
			D3D11_SDK_VERSION, &_device_ptr, &feature_level, &_contex_ptr);
		if (FAILED(hr))
			break;
		if (D3D_FEATURE_LEVEL_11_0 != feature_level)
			break;

		// Check 4X MSAA quality support
		hr = _device_ptr->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_m4xMsaa_quality);
		if (0 == _m4xMsaa_quality)
			break;

		// Fill DXGI_SWAP_CHAIN_DESC for swap chain.
		DXGI_SWAP_CHAIN_DESC desc;
		desc.BufferDesc.Width = _win_width;
		desc.BufferDesc.Height = _win_height;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		// 4x MSAA
		if (_enable4xMsaa) {
			desc.SampleDesc.Count = 4;
			desc.SampleDesc.Quality = _m4xMsaa_quality - 1;
		}
		else {
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = _wnd;
		desc.Windowed = true;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = 0;

		// Create swap chain
		IDXGIDevice *dxgi_device_ptr = NULL;
		hr = _device_ptr->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device_ptr);
		if (FAILED(hr))
			break;
		IDXGIAdapter *dxgi_adapter_ptr = NULL;
		hr = dxgi_device_ptr->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter_ptr);
		if (FAILED(hr))
			break;
		IDXGIFactory *dxgi_factory_ptr = NULL;
		hr = dxgi_adapter_ptr->GetParent(__uuidof(IDXGIFactory), (void**)&dxgi_factory_ptr);
		if (FAILED(hr))
			break;
		hr = dxgi_factory_ptr->CreateSwapChain(_device_ptr, &desc, &_swap_chain_ptr);
		if (FAILED(hr))
			break;
		dxgi_device_ptr->Release();
		dxgi_adapter_ptr->Release();
		dxgi_factory_ptr->Release();

		// The remaining steps are in on_resize...
		if (!on_resize())
			hr = E_FAIL;
	} while (false);

	return SUCCEEDED(hr);
}

