#include "mylib.h"
#include "macros.h"
#include "Hack.h"
#include "ProcMem.h"

ULONG_PTR MatchBin(BYTE buf[], size_t buflen, const BYTE pat[], 
				   size_t patlen, BYTE wildcard)
{
	size_t hint = 0;
	for (size_t i = 0; i < buflen; i ++) {

		if (buf[i] == pat[hint] || pat[hint] == wildcard)
			hint ++;
		else
			hint = 0;

		if (hint == patlen)
			return (ULONG_PTR)(i - patlen + 1);
	}

	return -1;
}

ULONG_PTR FastMatchBin(BYTE buf[], size_t buflen, const BYTE pat[], 
					   size_t patlen)
{
	size_t hint = 0;
	for (size_t i = 0; i < buflen; i ++) {

		if (buf[i] == pat[hint])
			hint ++;
		else
			hint = 0;

		if (hint == patlen)
			return (ULONG_PTR)(i - patlen + 1);
	}

	return -1;
}

bool LoadRemoteDll(DWORD pid, const char* dllPath)
{
	HANDLE hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | 
		PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, pid);

	if (hProc == NULL)
		return false;

	PVOID p = VirtualAllocEx(hProc, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	DWORD l;
	BOOL r = WriteProcessMemory(hProc, p, dllPath, strlen(dllPath) + 1, &l);

	if (!r) {

		VirtualFreeEx(hProc, p, strlen(dllPath) + 1, MEM_RELEASE);
		return false;
	}

	HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, 
		(LPTHREAD_START_ROUTINE )GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "LoadLibraryA"), 
		p, 0, &l);

	VirtualFreeEx(hProc, p, strlen(dllPath) + 1, MEM_RELEASE);

	if (hThread == NULL) {

		return false;
	}

	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, &l);
	CloseHandle(hThread);
	return l != 0;
}

bool UnloadRemoteDll(DWORD pid, const char* dllPath)
{
	HANDLE hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | 
		PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, pid);

	if (hProc == NULL)
		return false;

	PVOID p = VirtualAllocEx(hProc, NULL, strlen(dllPath) + 1, MEM_COMMIT, PAGE_READWRITE);
	DWORD l;
	BOOL r = WriteProcessMemory(hProc, p, dllPath, strlen(dllPath) + 1, &l);
	if (!r) {

		VirtualFreeEx(hProc, p, strlen(dllPath) + 1, MEM_RELEASE);
		return false;
	}

	HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, 
		(LPTHREAD_START_ROUTINE )GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "GetModuleHandleA"), 
		p, 0, &l);

	VirtualFreeEx(hProc, p, strlen(dllPath) + 1, MEM_RELEASE);

	if (hThread == NULL) {		
		return false;
	}

	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, &l);
	CloseHandle(hThread);

	if (l != NULL) {

		hThread = CreateRemoteThread(hProc, NULL, 0, 
			(LPTHREAD_START_ROUTINE )GetProcAddress(GetModuleHandle(_T("Kernel32.dll")), "FreeLibrary"), 
			CastPtr(l), 0, &l);

		if (hThread == NULL) {

			return false;
		}

		WaitForSingleObject(hThread, INFINITE);
		GetExitCodeThread(hThread, &l);
		CloseHandle(hThread);
		return l != 0;

	} else {
		return false;
	}
}

extern "C" {
	extern DWORD RpcStubLen;
	extern PVOID RpcStubCode;
	extern DWORD RpcStubProc;
	extern DWORD RpcStubParams;
	extern DWORD RpcStubParamCount;
	extern DWORD RpcStubConv;
	extern DWORD RpcStubSetThisPtr;
	extern DWORD RpcStubExitThread;
	// extern void __stdcall RpcStub(DWORD);
}

bool RemoteCall(HANDLE hProc, PVOID func, CallConvention conv, RpcParam params[], size_t pcount, PULONG_PTR ret)
{
	if (hProc == NULL)
		return false;

	ProcMem m;
	m.attach(hProc);

	size_t pstart = 0;
	if (conv == ConvThisCall) {
		pstart = 1;
		pcount --;
	}

	size_t stubStackSize = RpcStubLen + pcount * sizeof(ULONG_PTR);
	size_t remoteMemSize = 0;
	size_t paramSize = 0;

	ULONG_PTR stack[256];
	ULONG_PTR stackPos = 0;

	// 准备 stack
	for (size_t i = pstart; i < pstart + pcount; i ++) {
		// for (size_t i = pcount - 1; i >= 0; i --) {

		if (params[i]._isPtr) {

			// 需要列集的参数
			paramSize += params[i]._len;
		}

		stack[stackPos] = params[i]._stackFrame;
		stackPos ++;
	}

	// 计算需要的空间
	remoteMemSize = stubStackSize + paramSize;

	ULONG_PTR remoteMem = (ULONG_PTR )VirtualAllocEx(hProc, NULL, remoteMemSize, 
		MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (remoteMem == NULL)
		return false;

	// stub
	PBYTE stub = (PBYTE )malloc(RpcStubLen);
	memcpy(stub, ProbeProcAddr(RpcStubCode), RpcStubLen);
	*((PULONG_PTR )&stub[RpcStubParams]) = remoteMem + RpcStubLen;
	*((PULONG_PTR )&stub[RpcStubParamCount]) = pcount;
	*((PULONG_PTR )&stub[RpcStubProc]) = (ULONG_PTR )func;
	*((PULONG_PTR )&stub[RpcStubConv]) = conv == ConvCDecl ? 0 : 1;
	*((PULONG_PTR )&stub[RpcStubExitThread]) = (ULONG_PTR )GetProcAddress(
		LoadLibraryA("kernel32.dll"), "ExitThread");

	if (conv == ConvThisCall)
		*((PULONG_PTR )&stub[RpcStubSetThisPtr]) = params[0]._stackFrame;

	m.writeMem(remoteMem, RpcStubLen, stub);
	free(stub);

	// ptr param
	ULONG_PTR ptrParam = remoteMem + stubStackSize;
	for (size_t i = pstart; i < pstart + pcount; i ++) {

		if (params[i]._isPtr) {
			params[i]._remoteAddr = ptrParam;
			m.writeMem(ptrParam, params[i]._len, (PVOID )params[i]._stackFrame);
			ptrParam += params[i]._len;
		}
	}

	// adjust pointer
	stackPos = 0;
	for (size_t i = pstart; i < pstart + pcount; i ++) {
		// for (size_t i = pcount - 1; i >= 0; i --) {

		if (params[i]._isPtr) {
			stack[stackPos] = params[i]._remoteAddr;
		}

		stackPos ++;
	}

	// stack frame
	m.writeMem(remoteMem + RpcStubLen, pcount * sizeof(ULONG_PTR), stack);

	DWORD l;

	HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, 
		(LPTHREAD_START_ROUTINE )remoteMem, NULL, 0, &l);

	if (hThread == NULL) {

		VirtualFreeEx(hProc, (PVOID )remoteMem, remoteMemSize, MEM_RELEASE);
		return false;
	}

	WaitForSingleObject(hThread, INFINITE);
	GetExitCodeThread(hThread, &l);
	CloseHandle(hThread);

	PVOID result = malloc(paramSize);
	m.readMem(remoteMem + stubStackSize, paramSize, result);

	VirtualFreeEx(hProc, (PVOID )remoteMem, remoteMemSize, MEM_RELEASE);

	ptrParam = remoteMem + stubStackSize;
	for (size_t i = pstart; i < pstart + pcount; i ++) {

		if (params[i]._isPtr && params[i]._isOutParam) {

			// 从 result 中取出输出参数
			memcpy( (PVOID )params[i]._stackFrame, 
				(PVOID )((ULONG_PTR )result + (params[i]._remoteAddr - ptrParam) ), 
				params[i]._len );
		}
	}

	free(result);

	if (ret != NULL)
		*ret = l;

	return true;
}

HANDLE HackOpenProcess(DWORD pid)
{
	return OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | 
		PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, FALSE, pid);
}

bool RemoteCall(DWORD pid, PVOID func, CallConvention conv, RpcParam params[], size_t pcount, PULONG_PTR ret)
{
	HANDLE hProc = HackOpenProcess(pid);
	bool result = RemoteCall(hProc, func, conv, params, pcount, ret);
	CloseHandle(hProc);
	return result;
}

bool LocalCall(PVOID func, CallConvention conv, RpcParam params[], size_t pcount, PULONG_PTR ret)
{
	return RemoteCall(GetCurrentProcess(), func, conv, params, pcount, ret);
}

bool CallProc(PVOID func, CallConvention conv, RpcParam params[], size_t pcount, PULONG_PTR ret)
{
	int i;

	ULONG_PTR thisPtr = NULL;
	ULONG_PTR stackFrame;
	ULONG_PTR regEax;

	size_t pstart = 0;
	if (conv == ConvThisCall) {
		pstart = 1;
		pcount --;
		thisPtr = params[0]._stackFrame;
	}

	for (i = int(pstart + pcount - 1); i >= int(pstart) ; i --) {

		stackFrame = params[i]._stackFrame;
		_asm push stackFrame;
	}

	__try {
		__asm {
			mov ecx, thisPtr
			call func
			mov regEax, eax		
		}

	} __except(EXCEPTION_EXECUTE_HANDLER) {

		// SEH 会帮忙恢复栈
		return false;
	}

	if (conv == ConvCDecl) __asm {

		; release params
		mov eax, pcount
		shl eax, 2 ; multiply stack frame width
		add esp, eax
	}

	if (ret)
		*ret = regEax;

	return true;
}

void ApcThunk(CONTEXT ctx)
{
	__asm {
		push	eax
		call	edx ; stdcall
		mov		edi, ctx.Edi
		mov		esi, ctx.Esi
		mov		ebx, ctx.Ebx
		mov		edx, ctx.Edx
		mov		ecx, ctx.Ecx
		mov		eax, ctx.Eax		
		mov		esp, ctx.Esp
		push	ctx.Eip
		mov		ebp, ctx.Ebp
		ret		
	}
}

bool PostApc(HANDLE hThread, void* funcAddr, void* p)
{
	if (SuspendThread(hThread) == -1)
		return false;

	CONTEXT ctx;
	// ctx.ContextFlags = CONTEXT_ALL;
	ctx.ContextFlags = CONTEXT_FULL; // so you cannot calc for float
	GetThreadContext(hThread, &ctx);
	PCONTEXT pctx = (PCONTEXT )(ctx.Esp - sizeof(CONTEXT));
	*pctx = ctx;
	ctx.Esp -= sizeof(CONTEXT);
	// ctx.Esp -= sizeof(ULONG_PTR) * 2;
	ctx.Esp -= sizeof(ULONG_PTR);
	// *(ULONG_PTR* )(ctx.Esp + sizeof(ULONG_PTR))= (ULONG_PTR)pctx;
	*(ULONG_PTR* )ctx.Esp = 0;
	
	ctx.Eax = (ULONG_PTR)p;
	ctx.Edx = (ULONG_PTR)funcAddr;
	ctx.Eip = (ULONG_PTR)ApcThunk;
	ctx.ContextFlags = CONTEXT_FULL;
	if (!SetThreadContext(hThread, &ctx)) {
		ResumeThread(hThread);
		return false;
	}

	ResumeThread(hThread);
	return true;
}
