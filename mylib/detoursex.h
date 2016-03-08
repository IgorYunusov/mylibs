#pragma once

#include <atlbase.h>

#if defined(DETOURS_VERSION) && DETOURS_VERSION == 20100 

#define CreateProcessWithDll(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) \
	{ \
		USES_CONVERSION; \
		DetourCreateProcessWithDll(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, NULL, T2A(p11), p12); \
	}

inline PBYTE DetourFunction(PBYTE r, PBYTE m)
{
	DetourTransactionBegin();
	if (DetourAttach(&(PVOID& )r, m) != NO_ERROR)
		return NULL;
	DetourTransactionCommit();
	return r;
}

#pragma comment(lib, "detours.lib")
#pragma comment(lib, "detoured.lib")

#pragma comment(lib, "delayimp.lib")

#else
#define _DETOUR_TRAMPOLINE					DETOUR_TRAMPOLINE
#endif
