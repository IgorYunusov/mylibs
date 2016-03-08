#include "mylib.h"
#include "Utils/Utils.h"
#include "Optimize.h"

#include <d3d9.h>
#include <D3dx9core.h>
#pragma comment(lib, "D3D9.lib")
#pragma comment(lib, "D3Dx9.lib")

//////////////////////////////////////////////////////////////////////////

typedef IDirect3D9* (WINAPI * Direct3DCreate9_t)(UINT SDKVersion);

typedef HRESULT (WINAPI IDirect3D9::* IDirect3D9_CreateDevice_t)(
					 UINT Adapter,
					 D3DDEVTYPE DeviceType,
					 HWND hFocusWindow,
					 DWORD BehaviorFlags,
					 D3DPRESENT_PARAMETERS * pPresentationParameters,
					 IDirect3DDevice9 ** ppReturnedDeviceInterface
					 );

typedef HRESULT (WINAPI IDirect3DDevice9::* IDirect3DDevice9_Present_t )( CONST RECT * pSourceRect,
				CONST RECT * pDestRect,
				HWND hDestWindowOverride,
				CONST RGNDATA * pDirtyRegion
				);

typedef HRESULT (WINAPI IDirect3DDevice9::* IDirect3DDevice9_BeginScene_t)();
typedef HRESULT (WINAPI IDirect3DDevice9::* IDirect3DDevice9_BeginStateBlock_t)();
typedef HRESULT (WINAPI IDirect3DDevice9::* IDirect3DDevice9_SetTexture_t)(DWORD Stage,IDirect3DBaseTexture9* pTexture);

Direct3DCreate9_t Real_Direct3DCreate9 = NULL;
IDirect3D9_CreateDevice_t Real_IDirect3D9_CreateDevice = NULL;
IDirect3DDevice9_Present_t Real_IDirect3DDevice9_Present = NULL;
IDirect3DDevice9_BeginScene_t Real_IDirect3DDevice9_BeginScene = NULL;
IDirect3DDevice9_BeginStateBlock_t Real_IDirect3DDevice9_BeginStateBlock = NULL;
IDirect3DDevice9_SetTexture_t Real_IDirect3DDevice9_SetTexture = NULL;

volatile long present_delay_mstime = 200;
volatile bool present_wireframe = false;
volatile bool present_showfps = true;

const int PresentIndex = GetVirFuncIndex(&(PVOID &)IDirect3DDevice9::Present, ConvStdCall); // 17
//const int BeginSceneIndex = GetVirFuncIndex(&(PVOID &)IDirect3DDevice9::BeginScene, ConvStdCall); // 41
const int BeginStateBlockIndex = GetVirFuncIndex(&(PVOID &)IDirect3DDevice9::BeginStateBlock, ConvStdCall); // 60
const int SetTextureIndex = GetVirFuncIndex(&(PVOID &)IDirect3DDevice9::SetTexture, ConvStdCall); // 65

static int gFrameCount = 0;
static int gFrameCountTime = 0;
static long gFps = 0;

static ID3DXFont* g_pFont = NULL;

class IMyDirect3DDevice9: public IDirect3DDevice9 {
public:

	HRESULT WINAPI Mine_BeginScene()
	{
		_TRACE(__FUNCTION__);
		HRESULT hr = (this->*Real_IDirect3DDevice9_BeginScene)();
		return hr;
	}

	HRESULT WINAPI Mine_Present( CONST RECT * pSourceRect, 
		CONST RECT * pDestRect,
		HWND hDestWindowOverride,
		CONST RGNDATA * pDirtyRegion
		)
	{
		_TRACE(__FUNCTION__ ": ThreadId: %d", GetCurrentThreadId());
		if (present_delay_mstime)
			Sleep(present_delay_mstime);

		if (g_pFont == NULL) {

			D3DXCreateFont( this, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
				OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
				L"Arial", &g_pFont );
		}

		if (present_showfps && g_pFont != NULL) {

			gFrameCount ++;		

			if (GetTickCount() - gFrameCountTime >= 1000) {

				gFps = (long )ceil((float )gFrameCount / (float(GetTickCount() - gFrameCountTime) / 1000.0f));

				gFrameCountTime = GetTickCount();
				gFrameCount = 0;
			}

			RECT rc = {1, 1, 0, 0};
			char msg[64];
			sprintf(msg, "FPS: %d", gFps);
			g_pFont->DrawTextA(NULL, msg, -1, &rc, DT_NOCLIP, D3DXCOLOR(1, 1, 1, 1));
		}
		
		HRESULT hr = ( this->*Real_IDirect3DDevice9_Present )(pSourceRect, pDestRect, 
			hDestWindowOverride, pDirtyRegion);

		return hr;
	}

	HRESULT WINAPI Mine_BeginStateBlock()
	{
		_TRACE(__FUNCTION__);

		HRESULT hr = (this->*Real_IDirect3DDevice9_BeginStateBlock)();

		ULONG_PTR real;

		HookVirFunc(this, PresentIndex, &IMyDirect3DDevice9::Mine_Present, 
			real);

		//HookVirFunc(this, BeginSceneIndex, &IMyDirect3DDevice9::Mine_BeginScene, 
		//	real);

		HookVirFunc(this, BeginStateBlockIndex, &IMyDirect3DDevice9::Mine_BeginStateBlock, 
			real);

		HookVirFunc(this, SetTextureIndex, &IMyDirect3DDevice9::Mine_SetTexture, 
			real);

		return hr;
	};

	HRESULT WINAPI Mine_SetTexture(DWORD Stage,IDirect3DBaseTexture9* pTexture)
	{
		if (present_wireframe) {

			pTexture = NULL;
			SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		}

		HRESULT hr = (this->*Real_IDirect3DDevice9_SetTexture)(Stage, pTexture);
		
		return hr;
	}
};

class IMyDirect3D9: public IDirect3D9 {
public:

	HRESULT WINAPI Mine_CreateDevice(
		UINT Adapter,
		D3DDEVTYPE DeviceType,
		HWND hFocusWindow,
		DWORD BehaviorFlags,
		D3DPRESENT_PARAMETERS * pPresentationParameters,
		IDirect3DDevice9 ** ppReturnedDeviceInterface
		)
	{
		_TRACE(__FUNCTION__ ": W: %d, H: %d", pPresentationParameters->BackBufferWidth, pPresentationParameters->BackBufferHeight);

		//pPresentationParameters->BackBufferWidth = 800;
		//pPresentationParameters->BackBufferHeight = 600;
		HRESULT hr = (this->*Real_IDirect3D9_CreateDevice )(Adapter, DeviceType, hFocusWindow, BehaviorFlags, 
			pPresentationParameters, ppReturnedDeviceInterface);

		_TRACE(__FUNCTION__ ": Real_IDirect3D9_CreateDevice() = %08x, device = %p", hr, *ppReturnedDeviceInterface);

		// static volatile bool Hooked = false;
		if (SUCCEEDED(hr)) {

			// Hooked = true;
			ULONG_PTR real;

			HookVirFunc(*ppReturnedDeviceInterface, PresentIndex, &IMyDirect3DDevice9::Mine_Present, 
				real);

			if (Real_IDirect3DDevice9_Present == NULL)
				Cast(Real_IDirect3DDevice9_Present, real);

			_TRACE(__FUNCTION__ ": Real_IDirect3DDevice9_Present = %p, Index = %d", 
				Real_IDirect3DDevice9_Present, PresentIndex);

			//HookVirFunc(*ppReturnedDeviceInterface, BeginSceneIndex, &IMyDirect3DDevice9::Mine_BeginScene, 
			//	real);

			//if (Real_IDirect3DDevice9_BeginScene == NULL)
			//	Cast(Real_IDirect3DDevice9_BeginScene, real);

			// _TRACE(__FUNCTION__ ": Real_IDirect3DDevice9_BeginScene = %p", Real_IDirect3DDevice9_BeginScene);

			HookVirFunc(*ppReturnedDeviceInterface, BeginStateBlockIndex, &IMyDirect3DDevice9::Mine_BeginStateBlock, 
				real);

			if (Real_IDirect3DDevice9_BeginStateBlock == NULL)
				Cast(Real_IDirect3DDevice9_BeginStateBlock, real);

			_TRACE(__FUNCTION__ ": Real_IDirect3DDevice9_BeginStateBlock = %p, Index = %d", 
				Real_IDirect3DDevice9_BeginStateBlock, BeginStateBlockIndex);

			HookVirFunc(*ppReturnedDeviceInterface, SetTextureIndex, &IMyDirect3DDevice9::Mine_SetTexture, 
				real);

			if (Real_IDirect3DDevice9_SetTexture == NULL)
				Cast(Real_IDirect3DDevice9_SetTexture, real);

			_TRACE(__FUNCTION__ ": Real_IDirect3DDevice9_BeginStateBlock = %p, Index = %d", 
				Real_IDirect3DDevice9_SetTexture, SetTextureIndex);

			
			// (*ppReturnedDeviceInterface)->BeginScene();
			// (*ppReturnedDeviceInterface)->EndScene();
		}

		return hr;
	}
};

IDirect3D9* WINAPI Mine_Direct3DCreate9(UINT SDKVersion)
{
	_TRACE(__FUNCTION__);

	IDirect3D9* pID3d9 = Real_Direct3DCreate9(SDKVersion);
	_TRACE(__FUNCTION__ ": Real_Direct3DCreate9() = %p", pID3d9);

	// static volatile bool Hooked = false;

	if (pID3d9) {

		// Hooked = true;
		ULONG_PTR real;
		const int CreateDeviceIndex = 16;
		HookVirFunc(pID3d9, CreateDeviceIndex, &IMyDirect3D9::Mine_CreateDevice, real);
		if (Real_IDirect3D9_CreateDevice == NULL)
			Cast(Real_IDirect3D9_CreateDevice, real);

		_TRACE(__FUNCTION__ ": Real_IDirect3D9_CreateDevice = %p", Real_IDirect3D9_CreateDevice);
	}

	return pID3d9;
}

bool optimize()
{
	PCSTR DllName = "D3D9.DLL";
	if (LoadLibraryA(DllName) == NULL) {
		_TRACE(__FUNCTION__ ": LoadLibraryA(%s) failed(%d).", DllName, GetLastError());
		return false;
	}

	PBYTE tmp = (PBYTE )DetourFindFunction((PCHAR )DllName, "Direct3DCreate9");

	_TRACE( __FUNCTION__": Direct3DCreate9 = %p\n", tmp);

	if (tmp == NULL)
		return false;

	Real_Direct3DCreate9 = (Direct3DCreate9_t ) DetourFunction(
		(PBYTE )tmp, (PBYTE )Mine_Direct3DCreate9);

	_TRACE( (__FUNCTION__ ", Real_Direct3DCreate9 = %p, Mine_Direct3DCreate9 = %p\n"), 
		tmp, &Mine_Direct3DCreate9);

	return true;
}
