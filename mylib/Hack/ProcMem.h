#pragma once

#include "Utils/Log.h"

class ProcMem {
public:
	ProcMem()
	{
		_hProc = NULL;
	}

	void attach(HANDLE hProc)
	{
		assert(hProc != NULL);
		_hProc = hProc;
	}

	bool openProc(DWORD pid)
	{
		_hProc = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);
		if (_hProc == NULL)
			return false;

		return true;
	}

	void close()
	{
		if (_hProc)
			CloseHandle(_hProc);
	}

	size_t writeMem(PVOID addr, size_t len, LPCVOID buf)
	{
		size_t actlen;

		if (!WriteProcessMemory(_hProc, addr, buf, len, (SIZE_T* )&actlen)) {


			if (GetLastError() != ERROR_PARTIAL_COPY) {

				glog->log(LogWarning, "DbgEng::WriteMemory(Addr: %x, Size: %x) read failed. err: %x\n", 
					addr, len, GetLastError());
			}


			//DWORD prot;

			//if (!VirtualProtectEx(_hProc, addr, len, PAGE_READWRITE, &prot)) {

			//	glog->log(LogWarning, "DbgEng::WriteMemory(Addr: %x, Size: %x) prot failed. err: %x\n", 
			//		addr, len, GetLastError());
			//	return 0;
			//}

			//if (!WriteProcessMemory(_hProc, addr, buf, len, (SIZE_T* )&actlen)) {

			//	if (GetLastError() != ERROR_PARTIAL_COPY) {

			//		glog->log(LogWarning, "DbgEng::WriteMemory(Addr: %x, Size: %x) read failed. err: %x\n", 
			//			addr, len, GetLastError());
			//	}
			//}

			//if (!VirtualProtectEx(_hProc, addr, len, prot, &prot)) {

			//	glog->log(LogWarning, "DbgEng::WriteMemory(Addr: %x, Size: %x) prot failed. err: %x\n", 
			//		addr, len, GetLastError());
			//}
			return 0;
		}

		FlushInstructionCache(_hProc, addr, len);

		return actlen;
	}

	size_t writeMem(ULONG_PTR addr, size_t len, LPCVOID buf)
	{
		return writeMem((PVOID )addr, len ,buf);
	}

	size_t readMem(PVOID addr, size_t len, LPVOID buf)
	{
		SIZE_T rlen = 0;
		::ReadProcessMemory(_hProc, addr, buf, len, &rlen);
		return rlen;
	}

	size_t readMem(ULONG_PTR addr, size_t len, LPVOID buf)
	{
		return readMem((PVOID )addr, len, buf);
	}

	template <typename T>
	T readMem(PVOID addr)
	{
		T r;
		readMem(addr, sizeof(T), &r);
		return r;
	}

	template <typename T>
	T readMem(ULONG_PTR addr)
	{
		T r;
		readMem((PVOID )addr, sizeof(T), &r);
		return r;
	}
	
	template <typename T>
	size_t writeMem(PVOID addr, const T& w)
	{
		return writeMem(addr, sizeof(w), &w);
	}

	template <typename T>
	size_t writeMem(ULONG_PTR addr, const T& w)
	{
		return writeMem((PVOID )addr, sizeof(w), &w);
	}

	ULONG_PTR operator[] (ULONG_PTR addr)
	{
		return readMem<ULONG_PTR>((PVOID )addr);
	}

protected:
	HANDLE		_hProc;
};
