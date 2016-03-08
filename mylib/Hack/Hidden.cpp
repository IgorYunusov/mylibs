#include "mylib.h"
#include "macros.h"
#include "Hidden.h"

typedef struct _LDR_DATA_TABLE_ENTRY LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

struct _PEB_LDR_DATA {
	ULONG Length;
	UCHAR Initialized;
	ULONG_PTR SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
};

typedef struct _PEB {
	ULONG AllocationSize;
	ULONG Unknown1;
	HANDLE ProcessInstance;
	_PEB_LDR_DATA* Ldr;
	PVOID ProcessParameters;
	ULONG Unknown2;
	HANDLE DefaultHeap;

} PEB, *PPEB;

typedef struct _CLIENT_ID {
	ULONG UniqueProcess;
	ULONG UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

typedef struct _TEB {
	struct _EXCEPTION_REGISTRATION_RECORD *ExceptionList;
	PVOID StackBase;
	PVOID StackLimit;
	PVOID SubSystemTib;
	ULONG Version;
	PVOID ArbitraryUserPointer;
	struct _TEB *Self;

	ULONG Unknown1;
	CLIENT_ID ClientID;
	ULONG Unknown2;
	ULONG Unknown3;
	PPEB PebBaseAddress;
	ULONG LastError;
	ULONG Unknown[0x23];
	ULONG Locale;
	// ULONG ExitStack[EXIT_STACK_SIZE];

} TEB;

typedef TEB *PTEB;

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID DllBase;
	// ...
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

bool HideDll(LPCTSTR dllName)
{
	HMODULE hMod = GetModuleHandle(dllName);
	if (hMod == NULL) {

		// log warning
		return false;
	}

	PEB* Peb = NtCurrentTeb()->PebBaseAddress;

	PLIST_ENTRY ListHead, Current;
	PLDR_DATA_TABLE_ENTRY pstEntry = NULL;

	ListHead = &( Peb->Ldr->InLoadOrderModuleList);
	Current = ListHead->Flink;
	while ( Current != ListHead)
	{
		pstEntry = CONTAINING_RECORD( Current, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		if ( pstEntry->DllBase == hMod)
		{
			pstEntry->InLoadOrderLinks.Flink->Blink = pstEntry->InLoadOrderLinks.Blink;
			pstEntry->InLoadOrderLinks.Blink->Flink = pstEntry->InLoadOrderLinks.Flink;
			break;
		}
		Current = pstEntry->InLoadOrderLinks.Flink;
	}

	ListHead = &( Peb->Ldr->InMemoryOrderModuleList);
	Current = ListHead->Flink;
	while ( Current != ListHead)
	{
		pstEntry = CONTAINING_RECORD( Current, LDR_DATA_TABLE_ENTRY, InMemoryOrderModuleList);

		if ( pstEntry->DllBase == hMod)
		{
			pstEntry->InMemoryOrderModuleList.Flink->Blink = pstEntry->InMemoryOrderModuleList.Blink;
			pstEntry->InMemoryOrderModuleList.Blink->Flink = pstEntry->InMemoryOrderModuleList.Flink;
			break;
		}
		Current = pstEntry->InMemoryOrderModuleList.Flink;
	}

	ListHead = &( Peb->Ldr->InInitializationOrderModuleList);
	Current = ListHead->Flink;
	while ( Current != ListHead)
	{
		pstEntry = CONTAINING_RECORD( Current, LDR_DATA_TABLE_ENTRY, InInitializationOrderModuleList);
		if ( pstEntry->DllBase == hMod)
		{
			pstEntry->InInitializationOrderModuleList.Flink->Blink = pstEntry->InInitializationOrderModuleList.Blink;
			pstEntry->InInitializationOrderModuleList.Blink->Flink = pstEntry->InInitializationOrderModuleList.Flink;
			break;
		}
		Current = pstEntry->InInitializationOrderModuleList.Flink;
	}

	return true;
}

