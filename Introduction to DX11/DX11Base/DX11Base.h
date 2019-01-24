#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include "timer.h"


#define WINDOW_WIDTH	800
#define WINDOW_HEIGHT	600


class CDX11Base
{
public:
	CDX11Base();
	virtual ~CDX11Base();

	virtual bool init(HINSTANCE instance);
	virtual bool on_resize();
	virtual void update(float dt);
	virtual void draw();
	virtual LRESULT msg_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void on_mouse_pressed(WPARAM btnState, int x, int y);
	virtual void on_mouse_release(WPARAM btnState, int x, int y);
	virtual void on_mouse_move(WPARAM btnState, int x, int y);

	int run();

	HINSTANCE get_inst() const;
	HWND get_wnd() const;
	float get_ratio() const;

protected:
	bool _init_window();
	bool _init_d3d();

protected:
	HINSTANCE _inst;
	HWND _wnd;
	UINT32 _win_width;
	UINT32 _win_height;
	bool _enable4xMsaa;
	UINT _m4xMsaa_quality;
	bool _paused;
	bool _minimized;
	bool _maximized;
	bool _resizing;

	ID3D11Device *_device_ptr;
	ID3D11DeviceContext *_contex_ptr;
	IDXGISwapChain *_swap_chain_ptr;
	ID3D11RenderTargetView *_render_target_view_ptr;
	ID3D11DepthStencilView *_depth_stencil_view_ptr;
	ID3D11Texture2D *_depth_stencil_buffer_ptr;
};
