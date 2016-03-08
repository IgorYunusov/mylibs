#pragma once

#include "macros.h"

ULONG_PTR MatchBin(BYTE buf[], size_t buflen, const BYTE pat[], size_t patlen, BYTE wildcard);
ULONG_PTR FastMatchBin(BYTE buf[], size_t buflen, const BYTE pat[], size_t patlen);

enum CallConvention {
	ConvCDecl, 
	ConvStdCall,
	ConvThisCall, 
};

#define CastProcAddr(fn)		( &(PVOID& )fn )

#define ProbeProcAddr(fn)		( *PBYTE(fn) == 0xe9 ? \
	PBYTE( (ULONG_PTR)fn + *((PLONG )( (ULONG_PTR)fn + 1)) + 5 ) : PBYTE(fn) )

inline ULONG GetVirFuncIndex(PVOID fn, CallConvention conv)
{
	PBYTE p = (PBYTE )fn;

	// FOR DEBUG THUNK
	if (*p == 0xE9) {
		p = PBYTE((ULONG_PTR)p + *((PLONG )(p + 1)) + 5 );
	}

	if (conv == ConvStdCall)
		p += 6; // SKIP mov eax,dword ptr [esp+4] and mov eax,dword ptr [eax]
	else if (conv == ConvCDecl)
		p += 2; // SKIP mov eax,dword ptr [ecx]
	else
		return -1;

	if ( *(PWORD)p == 0xA0FF) { // jmp xxxxxxxx

		return *((PULONG )(p + 2))  >> 2;

	} else if (*(PWORD)p == 0x60FF) { // jmp xx

		return *((PBYTE )(p + 2)) >> 2;

	} else if (*(PWORD)p == 0x20FF) { // jmp dword ptr [eax]

		return 0;
	}

	return -1;
}

inline PVOID GetVirFuncAddr(PVOID obj, int index)
{
	PULONG_PTR vtbl = *(PULONG_PTR *)obj;
	return (PVOID )vtbl[index];
}

template<typename Mine, typename Real>
inline bool HookVirFunc(void* obj, int index, Mine mine, Real& real)
{
	PULONG_PTR vtbl = *(PULONG_PTR *)obj;
	DWORD dwOldProt;
	if (!VirtualProtect(&vtbl[index], sizeof(vtbl[index]), PAGE_READWRITE, &dwOldProt)) {

		_TRACE(__FUNCTION__ ": VirtualProtect() FAILED. LastErr: %d", GetLastError());
	}

	//real = Cast<Real>(vtbl[index]);
	//vtbl[index] = Cast<ULONG_PTR>(mine);

	Cast(real, vtbl[index]);
	Cast(vtbl[index], mine);

	return true;
}

template<typename Mine, typename Real>
inline bool HookVirFunc2(void* obj, int index, Mine mine, Real& real)
{
	Cast(real, GetVirFuncAddr(obj, index));
	DetourTransactionBegin();
	DetourAttach(&(PVOID& )real, Cast<PVOID>(mine));
	return DetourTransactionCommit() == 0;
}

bool LoadRemoteDll(DWORD pid, const char* dllPath);
bool UnloadRemoteDll(DWORD pid, const char* dllPath);

struct RpcParam {

	RpcParam()
	{
		_stackFrame = 0;
		_isPtr = false;
		_isOutParam = false;
		_len = 0;
		_remoteAddr = 0;
	}

	RpcParam(int stackFrame)
	{
		this->RpcParam::RpcParam((ULONG_PTR )stackFrame);
	}

	RpcParam(unsigned int stackFrame)
	{
		this->RpcParam::RpcParam((ULONG_PTR )stackFrame);
	}

	RpcParam(ULONG_PTR stackFrame)
	{
		_stackFrame = stackFrame;
		_isPtr = false;
		_isOutParam = false;
		_len = sizeof(ULONG_PTR);
		_remoteAddr = 0;
	}

	RpcParam(PULONG_PTR stackFrame)
	{
		_stackFrame = (ULONG_PTR )stackFrame;
		_isPtr = true;
		_isOutParam = true;
		_len = sizeof(ULONG_PTR);
		_remoteAddr = 0;
	}

	RpcParam(PLONG stackFrame)
	{
		_stackFrame = (ULONG_PTR )stackFrame;
		_isPtr = true;
		_isOutParam = true;
		_len = sizeof(LONG);
		_remoteAddr = 0;
	}

	RpcParam(PVOID stackFrame)
	{
		_stackFrame = (ULONG_PTR )stackFrame;
		_isPtr = false;
		_isOutParam = false;
		_len = sizeof(PVOID);
		_remoteAddr = 0;
	}

	RpcParam(ULONG_PTR stackFrame, size_t len, bool isOutParam)
	{
		_stackFrame = stackFrame;
		_isPtr = true;
		_isOutParam = isOutParam;
		_len = len;
		_remoteAddr = 0;
	}

	RpcParam(char* stackFrame, size_t len = 0)
	{
		_stackFrame = (ULONG_PTR )stackFrame;
		_isPtr = true;
		_isOutParam = true;
		_len = len == 0 ? strlen(stackFrame) + 1 : len;
		_remoteAddr = 0;		
	}

	RpcParam(const char* stackFrame)
	{
		_stackFrame = (ULONG_PTR )stackFrame;
		_isPtr = true;
		_isOutParam = false;
		_len = strlen(stackFrame) + 1;
		_remoteAddr = 0;		
	}

	RpcParam(wchar_t* stackFrame, size_t len = 0)
	{
		_stackFrame = (ULONG_PTR )stackFrame;
		_isPtr = true;
		_isOutParam = true;
		_len = len == 0 ? (wcslen(stackFrame) + 1 ) * sizeof(wchar_t) : len;
		_remoteAddr = 0;
	}

	RpcParam(const wchar_t* stackFrame)
	{
		_stackFrame = (ULONG_PTR )stackFrame;
		_isPtr = true;
		_isOutParam = false;
		_len = (wcslen(stackFrame) + 1) * sizeof(wchar_t);
		_remoteAddr = 0;
	}

	RpcParam(PVOID stackFrame, size_t len)
	{
		_stackFrame = (ULONG_PTR )stackFrame;
		_isPtr = true;
		_isOutParam = true;
		_len = len;
		_remoteAddr = 0;
	}

	RpcParam(LPCVOID stackFrame, size_t len)
	{
		_stackFrame = (ULONG_PTR )stackFrame;
		_isPtr = true;
		_isOutParam = false;
		_len = len;
		_remoteAddr = 0;
	}

	RpcParam(float stackFrame)
	{
		_stackFrame = *((PULONG_PTR )&stackFrame);
		_isPtr = false;
		_isOutParam = false;
		_len = sizeof(ULONG_PTR);
		_remoteAddr = 0;
	}

	ULONG_PTR		_stackFrame;	
	bool			_isPtr;
	bool			_isOutParam;
	size_t			_len;
	ULONG_PTR		_remoteAddr;
};

HANDLE HackOpenProcess(DWORD pid);
bool RemoteCall(HANDLE hProc, PVOID func, CallConvention conv, RpcParam params[], size_t pcount, PULONG_PTR ret = NULL);
bool RemoteCall(DWORD pid, PVOID func, CallConvention conv, RpcParam params[], size_t pcount, PULONG_PTR ret = NULL);
bool LocalCall(PVOID func, CallConvention conv, RpcParam params[], size_t pcount, PULONG_PTR ret = NULL);
bool CallProc(PVOID func, CallConvention conv, RpcParam params[], size_t pcount, PULONG_PTR ret = NULL);
bool PostApc(HANDLE hThread, void* funcAddr, void* p);
