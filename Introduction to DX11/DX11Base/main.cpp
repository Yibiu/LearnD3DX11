#include "dx11base.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	CDX11Base base;

	base.init(hInstance);
	return base.run();
}

