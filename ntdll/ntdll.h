//ntdll.h
//Copyright (c) 1997 Алексе?Коберниченко
//Типы, структур??функци? состав?ющие "родной" API Windows NT

#ifndef _NTDLL_H
#define _NTDLL_H

#ifdef  _MSC_VER
#pragma pack(push,8)
#endif  // _MSC_VER 

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifndef ANYSIZE_ARRAY
#define ANYSIZE_ARRAY 1       
#endif

#if (defined(_M_MRX000) || defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_PPC)) && !defined(MIDL_PASS)
#define DECLSPEC_IMPORT __declspec(dllimport)
#else
#define DECLSPEC_IMPORT
#endif

#if (defined(_M_MRX000) || defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_PPC)) && !defined(MIDL_PASS)
#define DECLSPEC_EXPORT __declspec(dllexport)
#else
#define DECLSPEC_EXPORT
#endif

#if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define NTAPI __stdcall
#else
#define _cdecl
#define NTAPI
#endif

#if !defined(_NTSYSTEM_)
#define NTSYSAPI DECLSPEC_IMPORT
#else
#define NTSYSAPI DECLSPEC_EXPORT
#endif

#ifndef CONST
#define CONST               const
#endif

typedef void *PVOID;    

#ifndef VOID
#define VOID void
typedef char CHAR;
typedef short SHORT;
typedef long LONG;
#endif

#define FALSE   0
#define TRUE    1

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif // NULL

//char ?wchar

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

typedef wchar_t WCHAR;   
typedef WCHAR *LPWSTR, *PWSTR;
typedef CONST WCHAR *LPCWSTR, *PCWSTR;

typedef CHAR *LPSTR, *PSTR;
typedef CONST CHAR *LPCSTR, *PCSTR;

#define UNICODE_NULL ((WCHAR)0)

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef UCHAR *PUCHAR;
typedef USHORT *PUSHORT;
typedef ULONG *PULONG;

typedef void *HANDLE;
typedef HANDLE *PHANDLE;
typedef UCHAR BOOLEAN;           
typedef BOOLEAN *PBOOLEAN;
typedef LONG *PLONG;
typedef long NTSTATUS;

#ifndef _WINNT_

typedef struct _LARGE_INTEGER {
   ULONG LowPart;
   LONG HighPart;
} LARGE_INTEGER,*PLARGE_INTEGER;

typedef struct _ULARGE_INTEGER {
   ULONG LowPart;
   ULONG HighPart;
} ULARGE_INTEGER,*PULARGE_INTEGER;

typedef LARGE_INTEGER LUID, *PLUID;

#endif

//Преобразование статус??ошибку

NTSYSAPI
ULONG
NTAPI
RtlNtStatusToDosError(
	IN NTSTATUS Status
	);

//Строка

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
    PSTR Buffer;
} STRING;
typedef STRING *PSTRING;

//Unicode-строка

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;

NTSYSAPI
VOID
NTAPI
RtlInitUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN PCWSTR SourceString
    );

//Атрибуты объект?

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_VALID_ATTRIBUTES    0x000000F2L

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        
    PVOID SecurityQualityOfService;  
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

#define OBJ_NAME_PATH_SEPARATOR ((WCHAR)L'\\')

//Маск?доступ?

typedef ULONG ACCESS_MASK;

#define DELETE                           (0x00010000L)
#define READ_CONTROL                     (0x00020000L)
#define WRITE_DAC                        (0x00040000L)
#define WRITE_OWNER                      (0x00080000L)
#define SYNCHRONIZE                      (0x00100000L)

#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define STANDARD_RIGHTS_READ             (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE            (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE          (READ_CONTROL)

#define STANDARD_RIGHTS_ALL              (0x001F0000L)

#define SPECIFIC_RIGHTS_ALL              (0x0000FFFFL)

#define ACCESS_SYSTEM_SECURITY           (0x01000000L)

#define MAXIMUM_ALLOWED                  (0x02000000L)

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

//Функци?для работы ?любы?объектом

NTSYSAPI
NTSTATUS
NTAPI
NtClose(
    IN HANDLE Handle
    );

NTSYSAPI
NTSTATUS
NTAPI
NtMakeTemporaryObject(
    IN HANDLE Handle
    );

#define DUPLICATE_CLOSE_SOURCE      0x00000001  
#define DUPLICATE_SAME_ACCESS       0x00000002  

NTSYSAPI
NTSTATUS 
NTAPI
NtDuplicateObject(
	IN HANDLE SourceProcessHandle,
	IN HANDLE SourceHandle,
	IN HANDLE TargetProcessHandle,
	OUT PHANDLE TargetHandle OPTIONAL,
	IN ACCESS_MASK DesiredAccess,
	IN ULONG Attributes,//OBJ_xxx 
	IN ULONG Options
	);

//Объект катало?

#define DIRECTORY_QUERY                 (0x0001)
#define DIRECTORY_TRAVERSE              (0x0002)
#define DIRECTORY_CREATE_OBJECT         (0x0004)
#define DIRECTORY_CREATE_SUBDIRECTORY   (0x0008)

#define DIRECTORY_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0xF)

NTSYSAPI
NTSTATUS
NTAPI
NtCreateDirectoryObject(
    OUT PHANDLE DirectoryHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS 
NTAPI
NtOpenDirectoryObject(
    OUT PHANDLE DirectoryHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

typedef struct _OBJECT_NAMETYPE_INFO {
	UNICODE_STRING ObjectName;
	UNICODE_STRING ObjectType;
} OBJECT_NAMETYPE_INFO, *POBJECT_NAMETYPE_INFO;

typedef enum _DIRECTORYINFOCLASS {
	ObjectArray,
	ObjectByOne
} DIRECTORYINFOCLASS, *PDIRECTORYINFOCLASS;

#define QUERY_DIRECTORY_BUF_SIZE 0x200

NTSYSAPI
NTSTATUS
NTAPI
NtQueryDirectoryObject(
	IN HANDLE DirectoryObjectHandle,
	OUT PVOID ObjectInfoBuffer,
	IN ULONG ObjectInfoBufferLength, 
	IN DIRECTORYINFOCLASS DirectoryInformationClass,
	IN BOOLEAN First, 
	IN OUT PULONG ObjectIndex,
	OUT PULONG LengthReturned
	);

//Объект символическая ссылка

#define SYMBOLIC_LINK_QUERY (0x0001)

#define SYMBOLIC_LINK_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | 0x1)

NTSYSAPI
NTSTATUS 
NTAPI
NtCreateSymbolicLinkObject(
    OUT PHANDLE ObjectHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN PUNICODE_STRING SubstituteString
    );

NTSYSAPI
NTSTATUS 
NTAPI
NtOpenSymbolicLinkObject(
    OUT PHANDLE ObjectHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

typedef struct _OBJECT_NAME_INFORMATION {               
    UNICODE_STRING Name;                                
} OBJECT_NAME_INFORMATION, *POBJECT_NAME_INFORMATION;   

NTSYSAPI
NTSTATUS 
NTAPI
NtQuerySymbolicLinkObject(
	IN HANDLE ObjectHandle,
	OUT POBJECT_NAME_INFORMATION SubstituteString,
	OUT PULONG SubstituteStringLength //?байтах
	);

//Объект событи?

#define EVENT_QUERY_STATE       0x0001 
#define EVENT_MODIFY_STATE      0x0002  
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) 

NTSYSAPI
NTSTATUS 
NTAPI
NtSetEvent(
	IN HANDLE EventHandle,
	IN PBOOLEAN OldState OPTIONAL
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtPulseEvent(
	IN HANDLE EventHandle,
	IN PBOOLEAN OldState OPTIONAL
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtResetEvent(
	IN HANDLE EventHandle,
	IN PBOOLEAN OldState OPTIONAL
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtClearEvent(
	IN HANDLE EventHandle
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtOpenEvent(
    OUT PHANDLE EventHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

typedef enum _EVENT_TYPE {
    NotificationEvent,	 //событи?со сбросо?вручну?
    SynchronizationEvent //событи??автоматическим сбросо?
    } EVENT_TYPE;

NTSYSAPI
NTSTATUS 
NTAPI
NtCreateEvent(
    OUT PHANDLE EventHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN EVENT_TYPE EventType,
	IN BOOLEAN InitialState 
	);

typedef enum _EVENTINFOCLASS {
	EventInfo
} EVENTINFOCLASS;

typedef struct _EVENT_INFO {
	EVENT_TYPE EventType;
	BOOLEAN Signaled;
} EVENT_INFO, *PEVENT_INFO;

NTSYSAPI
NTSTATUS 
NTAPI
NtQueryEvent(
	IN HANDLE EventHandle,
	IN EVENTINFOCLASS EventInformationClass,
	OUT PVOID EventInformation,
	IN ULONG EventInformationLength,
	OUT PULONG LengthReturned OPTIONAL 
	);

//Объект семафо?

#define SEMAPHORE_QUERY_STATE       0x0001	
#define SEMAPHORE_MODIFY_STATE      0x0002  
#define SEMAPHORE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) 

NTSYSAPI
NTSTATUS 
NTAPI
NtCreateSemaphore(
	OUT PHANDLE SemaphoreHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ULONG InitialCount,
	IN ULONG MaximumCount
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtOpenSemaphore(
    OUT PHANDLE SemaphoreHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS 
NTAPI
NtReleaseSemaphore(
	IN HANDLE SemaphoreHandle,
	IN ULONG ReleaseCount,
	OUT PULONG PreviousCount OPTIONAL
	);

typedef enum _SEMAPHOREINFOCLASS {
	SemaphoreInfo
} SEMAPHOREINFOCLASS;

typedef struct _SEMAPHORE_INFO {
	ULONG CurrentCount;
	ULONG MaximumCount;
} SEMAPHORE_INFO, *PSEMAPHORE_INFO;

NTSYSAPI
NTSTATUS 
NTAPI
NtQuerySemaphore(
	IN HANDLE SemaphoreHandle,
	IN SEMAPHOREINFOCLASS SemaphoreInformationClass,
	OUT PVOID SemaphoreInformation,
	IN ULONG SemaphoreInformationLength,
	OUT PULONG LengthReturned OPTIONAL 
	);

//Объект "мутант"

#define MUTANT_QUERY_STATE      0x0001

#define MUTANT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|\
                          MUTANT_QUERY_STATE)
NTSYSAPI
NTSTATUS 
NTAPI
NtCreateMutant(
	OUT PHANDLE MutantHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN BOOLEAN InitialOwner 
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtOpenMutant(
    OUT PHANDLE MutantHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS 
NTAPI
NtReleaseMutant(
	IN HANDLE MutantHandle,
	OUT PLONG OldOwnedCount OPTIONAL
	);

typedef enum _MUTANTINFOCLASS {
	MutantInfo
} MUTANTINFOCLASS;

typedef struct _MUTANT_INFO {
	LONG OwnedCount;
	BOOLEAN Owned;
} MUTANT_INFO, *PMUTANT_INFO;

NTSYSAPI
NTSTATUS 
NTAPI
NtQueryMutant(
	IN HANDLE MutantHandle,
	IN MUTANTINFOCLASS MutantInformationClass,
	OUT PVOID MutantInformation,
	IN ULONG MutantInformationLength,
	OUT PULONG LengthReturned OPTIONAL 
	);

//Функци?ожидан?

#define MAXIMUM_WAIT_OBJECTS 64

typedef struct _TIME { 
    ULONG LowTime; 
    LONG  HighTime; 
} TIME, *PTIME; 

NTSYSAPI
NTSTATUS 
NTAPI
NtWaitForSingleObject(
	IN HANDLE ObjectHandle,
	IN BOOLEAN Alertable, 
	IN PTIME Timeout OPTIONAL 
	);

typedef enum _WAIT_TYPE {
    WaitAll,
    WaitAny
    } WAIT_TYPE;

NTSYSAPI
NTSTATUS 
NTAPI
NtWaitForMultipleObjects(
	IN ULONG NumberOfHandles,
	IN PHANDLE ArrayOfHandles,
	IN WAIT_TYPE WaitType,
	IN BOOLEAN Alertable, 
	IN PTIME Timeout OPTIONAL 
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtDelayExecution(
	IN BOOLEAN Alertable, 
	IN PTIME Timeout 
	);

//Функци?работы ?предупрежден?ми

NTSYSAPI
NTSTATUS 
NTAPI
NtAlertThread(
	IN HANDLE ThreadHandle
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtTestAlert(
	);

//Объект таймер

#define TIMER_QUERY_STATE       0x0001	
#define TIMER_MODIFY_STATE      0x0002  
#define TIMER_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) 

#if (_WIN32_WINNT >= 0x0400)
typedef enum _TIMER_TYPE {
    NotificationTimer,	 //таймер со сбросо?вручну?
    SynchronizationTimer //таймер ?автоматическим сбросо?
    } TIMER_TYPE;
#endif //_WIN32_WINNT >= 0x0400

NTSYSAPI
NTSTATUS
NTAPI
NtCreateTimer(
    OUT PHANDLE TimerHandle,
    IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL
#if (_WIN32_WINNT >= 0x0400)
	,IN TIMER_TYPE TimerType 
#endif
	);

NTSYSAPI
NTSTATUS
NTAPI
NtOpenTimer(
    OUT PHANDLE TimerHandle,
    IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL
	);

NTSYSAPI
NTSTATUS
NTAPI
NtCancelTimer(
    IN HANDLE TimerHandle,
	IN PBOOLEAN OldState OPTIONAL
	);

#if !defined(_WINBASE) || (_WIN32_WINNT < 0400)

typedef
VOID
(NTAPI *PTIMERAPCROUTINE) (
    IN PVOID ApcContext,
    IN ULONG TimeLow,
    IN ULONG TimeHigh
    );

#endif

NTSYSAPI
NTSTATUS
NTAPI
NtSetTimer(
    IN HANDLE TimerHandle,
	IN PTIME DueTime,
	IN PTIMERAPCROUTINE TimerFunc OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	IN BOOLEAN Resume,
#if (_WIN32_WINNT >= 0x0400)
	IN LONG Period,	//?миллисекунда?
#endif
	IN PBOOLEAN OldState OPTIONAL 
	);

typedef enum _TIMERINFOCLASS {
	TimerInfo
} TIMERINFOCLASS;

typedef struct _TIMER_INFO { 
	TIME DueTime;
	BOOLEAN Signaled;
} TIMER_INFO, *PTIMER_INFO;

NTSYSAPI
NTSTATUS
NTAPI
NtQueryTimer(
	IN HANDLE TimerHandle,
	IN TIMERINFOCLASS TimerInformationClass,
	OUT PVOID TimerInformation,
	IN ULONG TimerInformationLength,
	OUT PULONG LengthReturned OPTIONAL 
	);

NTSYSAPI
NTSTATUS
NTAPI
NtSetTimerResolution(
	IN ULONG TimerResolution,
	IN BOOLEAN Set, 
	OUT PULONG SetTimerResolution 
	);

NTSYSAPI
NTSTATUS
NTAPI
NtQueryTimerResolution(
	OUT PULONG MaximumTimerResolution,
	OUT PULONG MinimumTimerResolution,
	OUT PULONG CurrentTimerResolution 
	);

//Время

typedef struct _TIME_FIELDS {
    USHORT Year;        // диапазон [1601...]
    USHORT Month;       // диапазон [1..12]
    USHORT Day;         // диапазон [1..31]
    USHORT Hour;        // диапазон [0..23]
    USHORT Minute;      // диапазон [0..59]
    USHORT Second;      // диапазон [0..59]
    USHORT Milliseconds;// диапазон [0..999]
    USHORT Weekday;     // диапазон [0..6] == [Воскресень?.Суббота]
} TIME_FIELDS;
typedef TIME_FIELDS *PTIME_FIELDS;

NTSYSAPI
VOID
NTAPI
RtlTimeToTimeFields (
    IN PTIME Time,
    OUT PTIME_FIELDS TimeFields
    );

NTSYSAPI
BOOLEAN
NTAPI
RtlTimeFieldsToTime (
    IN PTIME_FIELDS TimeFields,
    OUT PTIME Time
    );

NTSYSAPI
VOID
NTAPI
NtQuerySystemTime(
	OUT PTIME CurrentTime
	);

//Объект файл 

#define FILE_READ_DATA            ( 0x0001 )    // файл ?кана?
#define FILE_LIST_DIRECTORY       ( 0x0001 )    // катало?

#define FILE_WRITE_DATA           ( 0x0002 )    // файл ?кана?
#define FILE_ADD_FILE             ( 0x0002 )    // катало?

#define FILE_APPEND_DATA          ( 0x0004 )    // файл
#define FILE_ADD_SUBDIRECTORY     ( 0x0004 )    // катало?
#define FILE_CREATE_PIPE_INSTANCE ( 0x0004 )    // именованны?кана?

#define FILE_READ_EA              ( 0x0008 )    // файл ?катало?
#define FILE_READ_PROPERTIES      FILE_READ_EA

#define FILE_WRITE_EA             ( 0x0010 )    // файл ?катало?
#define FILE_WRITE_PROPERTIES     FILE_WRITE_EA

#define FILE_EXECUTE              ( 0x0020 )    // файл
#define FILE_TRAVERSE             ( 0x0020 )    // катало?

#define FILE_DELETE_CHILD         ( 0x0040 )    // катало?

#define FILE_READ_ATTRIBUTES      ( 0x0080 )    // вс?

#define FILE_WRITE_ATTRIBUTES     ( 0x0100 )    // вс?

#define FILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1FF)

#define FILE_GENERIC_READ         (STANDARD_RIGHTS_READ     |\
                                   FILE_READ_DATA           |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_READ_EA             |\
                                   SYNCHRONIZE)


#define FILE_GENERIC_WRITE        (STANDARD_RIGHTS_WRITE    |\
                                   FILE_WRITE_DATA          |\
                                   FILE_WRITE_ATTRIBUTES    |\
                                   FILE_WRITE_EA            |\
                                   FILE_APPEND_DATA         |\
                                   SYNCHRONIZE)


#define FILE_GENERIC_EXECUTE      (STANDARD_RIGHTS_EXECUTE  |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_EXECUTE             |\
                                   SYNCHRONIZE)

//
// Разделение доступ?
//

#define FILE_SHARE_READ                 0x00000001  
#define FILE_SHARE_WRITE                0x00000002  
#define FILE_SHARE_DELETE               0x00000004

//
// Атрибуты файл?
//

#define FILE_ATTRIBUTE_READONLY         0x00000001  
#define FILE_ATTRIBUTE_HIDDEN           0x00000002  
#define FILE_ATTRIBUTE_SYSTEM           0x00000004  
#define FILE_ATTRIBUTE_DIRECTORY        0x00000010  
#define FILE_ATTRIBUTE_ARCHIVE          0x00000020  
#define FILE_ATTRIBUTE_NORMAL           0x00000080  
#define FILE_ATTRIBUTE_TEMPORARY        0x00000100  
#define FILE_ATTRIBUTE_RESERVED0        0x00000200
#define FILE_ATTRIBUTE_RESERVED1        0x00000400
#define FILE_ATTRIBUTE_COMPRESSED       0x00000800  

#define FILE_CASE_SENSITIVE_SEARCH      0x00000001  
#define FILE_CASE_PRESERVED_NAMES       0x00000002  
#define FILE_UNICODE_ON_DISK            0x00000004  
#define FILE_PERSISTENT_ACLS            0x00000008  
#define FILE_FILE_COMPRESSION           0x00000010  
#define FILE_VOLUME_IS_COMPRESSED       0x00008000

#define FILE_ATTRIBUTE_VALID_FLAGS      0x00001fb7
#define FILE_ATTRIBUTE_VALID_SET_FLAGS  0x00000fa7

//
// Диспозиц?
//

#define FILE_SUPERSEDE                  0x00000000
#define FILE_OPEN                       0x00000001
#define FILE_CREATE                     0x00000002
#define FILE_OPEN_IF                    0x00000003
#define FILE_OVERWRITE                  0x00000004
#define FILE_OVERWRITE_IF               0x00000005
#define FILE_MAXIMUM_DISPOSITION        0x00000005

//
// Информац?, возвращаем? NtCreateFile/NtOpenFile
//

#define FILE_SUPERSEDED                 0x00000000
#define FILE_OPENED                     0x00000001
#define FILE_CREATED                    0x00000002
#define FILE_OVERWRITTEN                0x00000003
#define FILE_EXISTS                     0x00000004
#define FILE_DOES_NOT_EXIST             0x00000005

//
// Опци?создан?/открыт? файл?
//

#define FILE_DIRECTORY_FILE                     0x00000001
#define FILE_WRITE_THROUGH                      0x00000002
#define FILE_SEQUENTIAL_ONLY                    0x00000004
#define FILE_NO_INTERMEDIATE_BUFFERING          0x00000008

#define FILE_SYNCHRONOUS_IO_ALERT               0x00000010
#define FILE_SYNCHRONOUS_IO_NONALERT            0x00000020
#define FILE_NON_DIRECTORY_FILE                 0x00000040
#define FILE_CREATE_TREE_CONNECTION             0x00000080

#define FILE_COMPLETE_IF_OPLOCKED               0x00000100
#define FILE_NO_EA_KNOWLEDGE                    0x00000200
#define FILE_DISABLE_TUNNELING                  0x00000400
#define FILE_RANDOM_ACCESS                      0x00000800

#define FILE_DELETE_ON_CLOSE                    0x00001000
#define FILE_OPEN_BY_FILE_ID                    0x00002000
#define FILE_OPEN_FOR_BACKUP_INTENT             0x00004000
#define FILE_NO_COMPRESSION                     0x00008000

#define FILE_VALID_OPTION_FLAGS                 0x000FFFFF

#define FILE_VALID_SET_FLAGS                    0x00001036

//
// Максимальн? длин?имен?файл?
//

#define MAXIMUM_FILENAME_LENGTH         256

//
// Базовая структур? используем? во всех функция?ввод?вывода
//

typedef struct _IO_STATUS_BLOCK {
    NTSTATUS Status;
    ULONG Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

NTSYSAPI
NTSTATUS
NTAPI
NtCreateFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PLARGE_INTEGER AllocationSize OPTIONAL,
    IN ULONG FileAttributes,
    IN ULONG ShareAccess,
    IN ULONG CreateDisposition,
    IN ULONG CreateOptions,
    IN PVOID EaBuffer OPTIONAL,
    IN ULONG EaLength
    );

NTSYSAPI
NTSTATUS
NTAPI
NtOpenFile(
    OUT PHANDLE FileHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG ShareAccess,
    IN ULONG OpenOptions
	);

//
// Классы информации об устройстве ?файловой систем?
//

typedef enum _FSINFOCLASS {
    FileFsVolumeInformation = 1,
    FileFsLabelInformation,//2
    FileFsSizeInformation,//3
    FileFsDeviceInformation,//4
    FileFsAttributeInformation,//5
    FileFsQuotaQueryInformation,//6
    FileFsQuotaSetInformation,//7
    FileFsControlQueryInformation,//8
    FileFsControlSetInformation,//9
    FileFsMaximumInformation
} FS_INFORMATION_CLASS, *PFS_INFORMATION_CLASS;

typedef struct _FILE_FS_VOLUME_INFORMATION {
	TIME VolumeCreationTime;
	ULONG VolumeSerialNumber;
	ULONG VolumeNameSize;
	BOOLEAN CreationTimeSupported;
	UCHAR Spare;
	WCHAR VolumeName[ANYSIZE_ARRAY];
} FILE_FS_VOLUME_INFORMATION, *PFILE_FS_VOLUME_INFORMATION;     

typedef struct _FILE_FS_LABEL_INFORMATION {
	ULONG LabelNameSize;
	WCHAR LabelName[ANYSIZE_ARRAY];
} FILE_FS_LABEL_INFORMATION, *PFILE_FS_LABEL_INFORMATION;     

typedef struct _FILE_FS_SIZE_INFORMATION {
	ULARGE_INTEGER TotalNumberOfClusters;
	ULARGE_INTEGER NumberOfFreeClusters;
	ULONG SectorsPerCluster;
	ULONG BytesPerSector;
} FILE_FS_SIZE_INFORMATION, *PFILE_FS_SIZE_INFORMATION;     

#ifndef DEVICE_TYPE
#define DEVICE_TYPE ULONG
#endif

typedef struct _FILE_FS_DEVICE_INFORMATION {                    
    DEVICE_TYPE DeviceType;
    ULONG Characteristics;
} FILE_FS_DEVICE_INFORMATION, *PFILE_FS_DEVICE_INFORMATION;     
                                                                
typedef struct _FILE_FS_ATTRIBUTE_INFORMATION {
	ULONG FileSystemFlags;
	LONG MaximumComponentLength;
	ULONG FileSystemNameSize;
	WCHAR FileSystemName[ANYSIZE_ARRAY];
} FILE_FS_ATTRIBUTE_INFORMATION, *PFILE_FS_ATTRIBUTE_INFORMATION;
     
//Флаг?файловых систем

#define FS_CASE_IS_PRESERVED            FILE_CASE_PRESERVED_NAMES
#define FS_CASE_SENSITIVE               FILE_CASE_SENSITIVE_SEARCH
#define FS_UNICODE_STORED_ON_DISK       FILE_UNICODE_ON_DISK
#define FS_PERSISTENT_ACLS              FILE_PERSISTENT_ACLS
#define FS_VOL_IS_COMPRESSED            FILE_VOLUME_IS_COMPRESSED
#define FS_FILE_COMPRESSION             FILE_FILE_COMPRESSION

//
// Характеристики устройст?
//

#define FILE_REMOVABLE_MEDIA            0x00000001
#define FILE_READ_ONLY_DEVICE           0x00000002
#define FILE_FLOPPY_DISKETTE            0x00000004
#define FILE_WRITE_ONCE_MEDIA           0x00000008
#define FILE_REMOTE_DEVICE              0x00000010
#define FILE_DEVICE_IS_MOUNTED          0x00000020
#define FILE_VIRTUAL_VOLUME             0x00000040

// Типы устройст?

#define FILE_DEVICE_BEEP                0x00000001
#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM  0x00000003
#define FILE_DEVICE_CONTROLLER          0x00000004
#define FILE_DEVICE_DATALINK            0x00000005
#define FILE_DEVICE_DFS                 0x00000006
#define FILE_DEVICE_DISK                0x00000007
#define FILE_DEVICE_DISK_FILE_SYSTEM    0x00000008
#define FILE_DEVICE_FILE_SYSTEM         0x00000009
#define FILE_DEVICE_INPORT_PORT         0x0000000a
#define FILE_DEVICE_KEYBOARD            0x0000000b
#define FILE_DEVICE_MAILSLOT            0x0000000c
#define FILE_DEVICE_MIDI_IN             0x0000000d
#define FILE_DEVICE_MIDI_OUT            0x0000000e
#define FILE_DEVICE_MOUSE               0x0000000f
#define FILE_DEVICE_MULTI_UNC_PROVIDER  0x00000010
#define FILE_DEVICE_NAMED_PIPE          0x00000011
#define FILE_DEVICE_NETWORK             0x00000012
#define FILE_DEVICE_NETWORK_BROWSER     0x00000013
#define FILE_DEVICE_NETWORK_FILE_SYSTEM 0x00000014
#define FILE_DEVICE_NULL                0x00000015
#define FILE_DEVICE_PARALLEL_PORT       0x00000016
#define FILE_DEVICE_PHYSICAL_NETCARD    0x00000017
#define FILE_DEVICE_PRINTER             0x00000018
#define FILE_DEVICE_SCANNER             0x00000019
#define FILE_DEVICE_SERIAL_MOUSE_PORT   0x0000001a
#define FILE_DEVICE_SERIAL_PORT         0x0000001b
#define FILE_DEVICE_SCREEN              0x0000001c
#define FILE_DEVICE_SOUND               0x0000001d
#define FILE_DEVICE_STREAMS             0x0000001e
#define FILE_DEVICE_TAPE                0x0000001f
#define FILE_DEVICE_TAPE_FILE_SYSTEM    0x00000020
#define FILE_DEVICE_TRANSPORT           0x00000021
#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_DEVICE_VIDEO               0x00000023
#define FILE_DEVICE_VIRTUAL_DISK        0x00000024
#define FILE_DEVICE_WAVE_IN             0x00000025
#define FILE_DEVICE_WAVE_OUT            0x00000026
#define FILE_DEVICE_8042_PORT           0x00000027
#define FILE_DEVICE_NETWORK_REDIRECTOR  0x00000028
#define FILE_DEVICE_BATTERY             0x00000029
#define FILE_DEVICE_BUS_EXTENDER        0x0000002a

//
// Методы передачи буферо?
//

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

// Макрос для определения кодо?управлен?

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)


#define FILE_ANY_ACCESS                 0
#define FILE_READ_ACCESS          ( 0x0001 )    // файл ?кана?
#define FILE_WRITE_ACCESS         ( 0x0002 )    // файл ?кана?


NTSYSAPI
NTSTATUS
NTAPI
NtQueryVolumeInformationFile(
	IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID VolumeInformation,
    IN ULONG Length,
	IN FS_INFORMATION_CLASS FsInformationClass
	);

NTSYSAPI
NTSTATUS
NTAPI
NtSetVolumeInformationFile(
	IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID VolumeInformation,
    IN ULONG Length,
	IN FS_INFORMATION_CLASS FsInformationClass
	);

typedef
VOID
(NTAPI *PIO_APC_ROUTINE) (
    IN PVOID ApcContext,
    IN PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG Reserved
    );

NTSYSAPI
NTSTATUS 
NTAPI
NtDeviceIoControlFile(
	IN HANDLE DeviceHandle,
	IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
	IN OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG  IoControlCode,
	IN PVOID  InBuffer,	
    IN ULONG  InBufferSize,
    OUT PVOID  OutBuffer,
    IN ULONG  OutBufferSize
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtFsControlFile(
	IN HANDLE DeviceHandle,
	IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
	IN OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN ULONG  IoControlCode,
	IN PVOID  InBuffer,	
    IN ULONG  InBufferSize,
    OUT PVOID  OutBuffer,
    IN ULONG  OutBufferSize
	);

#define FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001   
#define FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002   
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004   
#define FILE_NOTIFY_CHANGE_SIZE         0x00000008   
#define FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010   
#define FILE_NOTIFY_CHANGE_SECURITY     0x00000100   

#define FILE_ACTION_ADDED				0x00000001
#define FILE_ACTION_REMOVED				0x00000002
#define FILE_ACTION_MODIFIED			0x00000003
#define FILE_ACTION_RENAMED_OLD_NAME	0x00000004
#define FILE_ACTION_RENAMED_NEW_NAME	0x00000005

#if !defined(_WINNT_) || (_MSC_VER < 1000)

typedef struct _FILE_NOTIFY_INFORMATION {
	ULONG NextEntryOffset;
	ULONG Action;
	ULONG FileNameLength;
	WCHAR FileName[1];
}FILE_NOTIFY_INFORMATION;

#endif

NTSYSAPI
NTSTATUS
NTAPI
NtNotifyChangeDirectoryFile(
	IN HANDLE DirectoryHandle,
	IN HANDLE EventHandle OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
	IN PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID Buffer,
	IN ULONG BufferLength,
	IN ULONG NotifyFiltr,//FILE_NOTIFY_CHANGE_xxx
	IN BOOLEAN WatchSubtree
	);

// Классы информации ?файл?

typedef enum _FILE_INFORMATION_CLASS {
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation,//2
    FileBothDirectoryInformation,//3
    FileBasicInformation,//4
    FileStandardInformation,//5
    FileInternalInformation,//6
    FileEaInformation,//7
    FileAccessInformation,//8
    FileNameInformation,//9
    FileRenameInformation,//10 (0Ah)
    FileLinkInformation,//11 (0Bh)
    FileNamesInformation,//12 (0Ch)
    FileDispositionInformation,//13 (0Dh)
    FilePositionInformation,//14 (0Eh)
    FileFullEaInformation,//15 (0Fh)
    FileModeInformation,//16 (10h)
    FileAlignmentInformation,//17 (11h)
    FileAllInformation,//18 (12h)
    FileAllocationInformation,//19 (13h)
    FileEndOfFileInformation,//20 (14h)
    FileAlternateNameInformation,//21 (15h)
    FileStreamInformation,//22 (16h)
    FilePipeInformation,//23 (17h)
    FilePipeLocalInformation,//24 (18h)
    FilePipeRemoteInformation,//25 (19h)
    FileMailslotQueryInformation,//26 (1Ah)
    FileMailslotSetInformation,//27 (1Bh)
    FileCompressionInformation,//28 (1Ch)
    FileCopyOnWriteInformation,//29 (1Dh)
    FileCompletionInformation,//30 (1Eh)
    FileMoveClusterInformation,//31 (1Fh)
    FileOleClassIdInformation,//32 (20h)
    FileOleStateBitsInformation,//33 (21h)
    FileApplicationExplorableInformation,//34 (22h)
    FileApplicationExplorableChildrenInformation,//35 (23h)
    FileObjectIdInformation,//36 (24h)
    FileOleAllInformation,//37 (25h)
    FileOleDirectoryInformation,//38 (26h)
    FileTransactionCommitInformation,//39 (27h)
    FileContentIndexInformation,//40 (28h)
    FileInheritContentIndexInformation,//41 (29h)
    FileOleInformation,//42 (2Ah)
    FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef struct _FILE_QUERY_DIRECTORY { 
	ULONG NextEntryOffset;
	ULONG Unknown1;
	TIME CreationTime;
	TIME LastAccessTime;
	TIME LastWriteTime;
	TIME ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
	ULONG FileNameLength;
	union {
		struct {
			WCHAR FileName[ANYSIZE_ARRAY];
		} Class1;
		struct {
			ULONG Unknown2;
			WCHAR FileName[ANYSIZE_ARRAY];
		} Class2;
		struct {
			ULONG Unknown2;
			USHORT AlternateFileNameLength;
			WCHAR AlternateFileName[12];
			WCHAR FileName[ANYSIZE_ARRAY];
		} Class3;
	};
} FILE_QUERY_DIRECTORY, *PFILE_QUERY_DIRECTORY;

NTSYSAPI
NTSTATUS
NTAPI
NtQueryDirectoryFile(
	IN HANDLE DirectoryFileHandle,
	IN HANDLE EventHandle OPTIONAL,
	IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID Buffer,
	IN ULONG BufferLength,
	IN FILE_INFORMATION_CLASS DirectoryInfoClass,
	IN BOOLEAN ByOne,
	IN PUNICODE_STRING SearchTemplate OPTIONAL,
	IN BOOLEAN Reset
	);

typedef struct _FILE_BASIC_INFORMATION {                    
    LARGE_INTEGER CreationTime;                             
    LARGE_INTEGER LastAccessTime;                           
    LARGE_INTEGER LastWriteTime;                            
    LARGE_INTEGER ChangeTime;                               
    ULONG FileAttributes;                                   
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;         
                                                            
typedef struct _FILE_STANDARD_INFORMATION {                 
    LARGE_INTEGER AllocationSize;                           
    LARGE_INTEGER EndOfFile;                                
    ULONG NumberOfLinks;                                    
    BOOLEAN DeletePending;                                  
    BOOLEAN Directory;                                      
} FILE_STANDARD_INFORMATION, *PFILE_STANDARD_INFORMATION;   
                                                            
#include <pshpack4.h>
typedef struct _FILE_EA_INFORMATION {
	ULONG EaLength; 
} FILE_EA_INFORMATION, *PFILE_EA_INFORMATION;

typedef struct _FILE_ACCESS_INFORMATION {
	ACCESS_MASK GrantedAccess;
} FILE_ACCESS_INFORMATION, *PFILE_ACCESS_INFORMATION;
#include <poppack.h>

typedef struct _FILE_NAME_INFORMATION {
	ULONG NameSize;
	WCHAR Name[ANYSIZE_ARRAY];
} FILE_NAME_INFORMATION, *PFILE_NAME_INFORMATION;

#include <pshpack1.h>
typedef struct _FILE_DISPOSITION_INFORMATION {                  
    BOOLEAN DeleteFile;                                         
} FILE_DISPOSITION_INFORMATION, *PFILE_DISPOSITION_INFORMATION; 
#include <poppack.h>
                                                                
typedef struct _FILE_POSITION_INFORMATION {                 
    LARGE_INTEGER CurrentByteOffset;                        
} FILE_POSITION_INFORMATION, *PFILE_POSITION_INFORMATION;   
                                                            
typedef struct _FILE_FULL_EA_INFORMATION {
    ULONG NextEntryOffset;
    UCHAR Flags;
    UCHAR EaNameLength;
    USHORT EaValueLength;
    CHAR EaName[1];//за не?следуе?EaValue
	//CHAR EaValue[1];
} FILE_FULL_EA_INFORMATION, *PFILE_FULL_EA_INFORMATION;

typedef struct _FILE_MODE_INFORMATION {                
    ULONG Mode;                             
} FILE_MODE_INFORMATION, *PFILE_MODE_INFORMATION; 
                                                            
//
// Выравнивание
//

#define FILE_BYTE_ALIGNMENT             0x00000000
#define FILE_WORD_ALIGNMENT             0x00000001
#define FILE_LONG_ALIGNMENT             0x00000003
#define FILE_QUAD_ALIGNMENT             0x00000007
#define FILE_OCTA_ALIGNMENT             0x0000000f
#define FILE_32_BYTE_ALIGNMENT          0x0000001f
#define FILE_64_BYTE_ALIGNMENT          0x0000003f
#define FILE_128_BYTE_ALIGNMENT         0x0000007f
#define FILE_256_BYTE_ALIGNMENT         0x000000ff
#define FILE_512_BYTE_ALIGNMENT         0x000001ff

typedef struct _FILE_ALIGNMENT_INFORMATION {                
    ULONG AlignmentRequirement;                             
} FILE_ALIGNMENT_INFORMATION, *PFILE_ALIGNMENT_INFORMATION; 
                                                            
typedef struct _FILE_END_OF_FILE_INFORMATION {                  
    LARGE_INTEGER EndOfFile;                                    
} FILE_END_OF_FILE_INFORMATION, *PFILE_END_OF_FILE_INFORMATION; 
                                                                
typedef struct _FILE_ALTERNATE_NAME_INFORMATION {
	ULONG NameSize;
	WCHAR Name[ANYSIZE_ARRAY];
} FILE_ALTERNATE_NAME_INFORMATION, *PFILE_ALTERNATE_NAME_INFORMATION;

typedef struct _FILE_STREAM_INFORMATION {
    ULONG NextEntryOffset;
	ULONG StreamNameLength;
	LARGE_INTEGER StreamLength;
	LARGE_INTEGER AllocationSize;
	WCHAR StreamName[ANYSIZE_ARRAY];
} FILE_STREAM_INFORMATION, *PFILE_STREAM_INFORMATION;

#define COMPRESSION_FORMAT_NONE          (0x0000)   
#define COMPRESSION_FORMAT_DEFAULT       (0x0001)   
#define COMPRESSION_FORMAT_LZNT1         (0x0002)   

typedef struct _FILE_COMPRESSION_INFORMATION {
	LARGE_INTEGER CompressedSize;
	USHORT CompressionFormat; //COMPRESSION_FORMAT_xxx
} FILE_COMPRESSION_INFORMATION, *PFILE_COMPRESSION_INFORMATION;

typedef struct _FILE_COMPLETION_INFORMATION {
	HANDLE CompletionPort;
	ULONG CompletionKey;
} FILE_COMPLETION_INFORMATION, *PFILE_COMPLETION_INFORMATION;

NTSYSAPI
NTSTATUS
NTAPI
NtQueryEaFile(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID Buffer,
    IN ULONG BufferLength,
	IN BOOLEAN ByOne,
	IN PVOID EaList OPTIONAL,
	IN ULONG EaListLength,
	IN PVOID EaName OPTIONAL,
	IN BOOLEAN Reset 
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtSetEaFile(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID EaBuffer,
	IN ULONG EaLength
	);

NTSYSAPI
NTSTATUS
NTAPI
NtQueryInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
    );

NTSYSAPI
NTSTATUS
NTAPI
NtSetInformationFile(
    IN HANDLE FileHandle,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID FileInformation,
    IN ULONG Length,
    IN FILE_INFORMATION_CLASS FileInformationClass
    );

//
// Специальны?значен? смещен?
//

#define FILE_WRITE_TO_END_OF_FILE       0xffffffff
#define FILE_USE_FILE_POINTER_POSITION  0xfffffffe

NTSYSAPI
NTSTATUS
NTAPI
NtReadFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    OUT PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL,
    IN PULONG Key OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
NtWriteFile(
    IN HANDLE FileHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN PVOID Buffer,
    IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL,
    IN PULONG Key OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
NtQueryAttributesFile(
	IN POBJECT_ATTRIBUTES FileObjectAttributes,
	OUT PFILE_BASIC_INFORMATION FileAttributes
	);

#if (_WIN32_WINNT>=0x0400)

typedef struct _FULL_FILE_ATTRIBUTES { 
	TIME CreationTime;
	TIME LastAccessTime;
	TIME LastWriteTime;
	TIME ChangeTime;
	LARGE_INTEGER EndOfFile;
	LARGE_INTEGER AllocationSize;
	ULONG FileAttributes;
} FULL_FILE_ATTRIBUTES, *PFULL_FILE_ATTRIBUTES;

NTSYSAPI
NTSTATUS
NTAPI
NtQueryFullAttributesFile(
	IN POBJECT_ATTRIBUTES FileObjectAttributes,
	OUT PFULL_FILE_ATTRIBUTES FullFileAttributes
	);

#endif //_WIN32_WINNT >= 0x0400

NTSYSAPI
NTSTATUS 
NTAPI
NtDeleteFile(
	IN POBJECT_ATTRIBUTES FileObjectAttributes
	);

//Вспомогательны?функци?

typedef enum _DOS_PATHNAME_TYPE {
	Path_UNCFullName=1,// "\\x" ил?"\\.x" 
	Path_DeviceRootDir,// "x:\" 
	Path_StreamName,// "x:x" 
	Path_RelativeDir,// "\x" 
	Path_RelativeName,// "xx" 
	Path_UNCLocalName,// "\\.\"	
	Path_UNCLocalRoot // "\\.0" 
} DOS_PATHNAME_TYPE, *PDOS_PATHNAME_TYPE;

NTSYSAPI                                            
DOS_PATHNAME_TYPE
NTAPI                                               
RtlDetermineDosPathNameType_U(
	IN PCWSTR PathName
	);

typedef struct _RELATIVE_NAME {
	UNICODE_STRING Name;
	HANDLE CurrentDir;
} RELATIVE_NAME, *PRELATIVE_NAME;

NTSYSAPI
BOOLEAN 
NTAPI
RtlDosPathNameToNtPathName_U(
	IN PCWSTR DosPathName,
	OUT PUNICODE_STRING NtPathName,
	OUT PWSTR* FilePartInNtPathName OPTIONAL,
	OUT PRELATIVE_NAME RelativeName OPTIONAL
	);

NTSYSAPI
NTSTATUS
NTAPI
RtlNtPathNameToDosPathName(
						   ULONG Flags,
						   PUNICODE_STRING Path,
						   PULONG Disposition,
						   PWSTR* FilePart);

//Порт завершен? операций ввод?вывода

#define IO_COMPLETION_QUERY_STATE  0x0001
#define IO_COMPLETION_MODIFY_STATE  0x0002  
#define IO_COMPLETION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) 

NTSYSAPI
NTSTATUS 
NTAPI
NtCreateIoCompletion(
	OUT PHANDLE IoCompletionPortHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN ULONG NumberOfConcurrentThreads
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtOpenIoCompletion(
    OUT PHANDLE IoCompletionPortHandle,
    IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS 
NTAPI
NtSetIoCompletion(
	IN HANDLE IoCompletionPortHandle,
	IN ULONG CompletionKey, //отдельная величина на каждый файл
	IN ULONG CompletionValue,//отдельная величина на каждую операцию
	IN NTSTATUS CompletionStatus,
	IN ULONG CompletionInformation
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtRemoveIoCompletion(
	IN HANDLE IoCompletionPortHandle,
	OUT PULONG CompletionKey, //отдельная величина на каждый файл
	OUT PULONG CompletionValue,//отдельная величина на каждую операцию
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PTIME Timeout OPTIONAL 
	);

typedef enum _IOCOMPLETION_INFORMATION_CLASS {
	IoCompletionInfo
} IOCOMPLETION_INFORMATION_CLASS, *PIOCOMPLETION_INFORMATION_CLASS;

#include <pshpack4.h>
typedef struct _IOCOMPLETION_INFO {
	ULONG QueueLength;
} IOCOMPLETION_INFO, *PIOCOMPLETION_INFO;
#include <poppack.h>

NTSYSAPI
NTSTATUS 
NTAPI
NtQueryIoCompletion(
	IN HANDLE IoCompletionPortHandle,
	IN IOCOMPLETION_INFORMATION_CLASS IoCompletionInformationClass,
    OUT PVOID IoCompletionInformation,
    IN ULONG Length,
	OUT PULONG LengthReturned OPTIONAL
	);

//Виртуальная па?ть

#define PAGE_NOACCESS          0x01     
#define PAGE_READONLY          0x02     
#define PAGE_READWRITE         0x04     
#define PAGE_WRITECOPY         0x08     
#define PAGE_EXECUTE           0x10     
#define PAGE_EXECUTE_READ      0x20     
#define PAGE_EXECUTE_READWRITE 0x40     
#define PAGE_EXECUTE_WRITECOPY 0x80     
#define PAGE_GUARD            0x100     
#define PAGE_NOCACHE          0x200     

#define MEM_COMMIT           0x1000     
#define MEM_RESERVE          0x2000     
#define MEM_DECOMMIT         0x4000     
#define MEM_RELEASE          0x8000     
#define MEM_FREE            0x10000     
#define MEM_PRIVATE         0x20000     
#define MEM_MAPPED          0x40000     
#define MEM_TOP_DOWN       0x100000     
#define MEM_LARGE_PAGES  0x20000000
     
#define SEC_FILE           0x800000     
#define SEC_IMAGE         0x1000000     
#define SEC_RESERVE       0x4000000     
#define SEC_COMMIT        0x8000000     
#define SEC_NOCACHE      0x10000000     
#define MEM_IMAGE         SEC_IMAGE

NTSYSAPI
NTSTATUS 
NTAPI
NtAllocateVirtualMemory(
	IN HANDLE ProcessHandle,
	IN OUT PVOID *RegionAddress,
	IN ULONG ZeroBits,
	IN OUT PULONG RegionSize,
	IN ULONG AllocationType,
	IN ULONG ProtectionType
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtFreeVirtualMemory(
	IN HANDLE ProcessHandle,
	IN OUT PVOID *RegionAddress,
	IN OUT PULONG RegionSize,
	IN ULONG FreeType
	);

#define UNLOCK_TYPE_NON_PRIVILEGED	0x00000001L
#define UNLOCK_TYPE_PRIVILEGED		0x00000002L

NTSYSAPI
NTSTATUS 
NTAPI
NtLockVirtualMemory(
	IN HANDLE ProcessHandle,
	IN OUT PVOID *RegionAddress,
	IN OUT PULONG RegionSize,
	IN ULONG UnlockTypeRequired
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtUnlockVirtualMemory(
	IN HANDLE ProcessHandle,
	IN OUT PVOID *RegionAddress,
	IN OUT PULONG RegionSize,
	IN ULONG UnlockTypeRequiested
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtReadVirtualMemory(
	IN HANDLE ProcessHandle,
	IN PVOID StartAddress,
	OUT PVOID Buffer,
	IN ULONG BytesToRead,
	OUT PULONG BytesReaded OPTIONAL
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtWriteVirtualMemory(
	IN HANDLE ProcessHandle,
	IN PVOID StartAddress,
	IN PVOID Buffer,
	IN ULONG BytesToWrite,
	OUT PULONG BytesWritten OPTIONAL
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtProtectVirtualMemory(
	IN HANDLE ProcessHandle,
	IN OUT PVOID *RegionAddress,
	IN OUT PULONG RegionSize,
	IN ULONG DesiredProtection,
	OUT PULONG OldProtection
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtFlushVirtualMemory(
	IN HANDLE ProcessHandle,
	IN PVOID* StartAddress,
	IN PULONG BytesToFlush,
	OUT PIO_STATUS_BLOCK StatusBlock 
	);

typedef enum _MEMORYINFOCLASS {
	MemoryBasicInformation,
	MemoryEntryInformation
} MEMORYINFOCLASS;

#ifndef _WINNT_

typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase;
    ULONG AllocationProtect;
    ULONG RegionSize;
    ULONG State;
    ULONG Protect;
    ULONG Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

#endif

typedef struct _MEMORY_ENTRY_INFORMATION {
	ULONG NumberOfEntries;
	PVOID Entries[ANYSIZE_ARRAY];
} MEMORY_ENTRY_INFORMATION, *PMEMORY_ENTRY_INFORMATION;

NTSYSAPI
NTSTATUS 
NTAPI
NtQueryVirtualMemory(
	IN HANDLE ProcessHandle,
	IN PVOID RegionAddress,
	IN MEMORYINFOCLASS MemoryInformationClass,
	IN PVOID VirtualMemoryInfo,
	IN ULONG Length,
	OUT PULONG ActualLength OPTIONAL
	);

//Куча

#define HEAP_NO_SERIALIZE               0x00000001      
#define HEAP_GROWABLE                   0x00000002      
#define HEAP_GENERATE_EXCEPTIONS        0x00000004      
#define HEAP_ZERO_MEMORY                0x00000008      
#define HEAP_REALLOC_IN_PLACE_ONLY      0x00000010      
#define HEAP_TAIL_CHECKING_ENABLED      0x00000020      
#define HEAP_FREE_CHECKING_ENABLED      0x00000040      
#define HEAP_DISABLE_COALESCE_ON_FREE   0x00000080      
#define HEAP_CREATE_ALIGN_16            0x00010000      
#define HEAP_CREATE_ENABLE_TRACING      0x00020000      
#define HEAP_MAXIMUM_TAG                0x0FFF              
#define HEAP_PSEUDO_TAG_FLAG            0x8000              
#define HEAP_TAG_SHIFT                  16                  
#define HEAP_MAKE_TAG_FLAGS( b, o ) ((DWORD)((b) + ((o) << 16))) 

typedef PVOID PHEAP_INFO;

NTSYSAPI
HANDLE 
NTAPI
RtlCreateHeap(
	IN ULONG AllocationFlags,
	IN PVOID BaseAddress OPTIONAL,
	IN ULONG MaximumSize,
	IN ULONG InitialSize,
	IN PVOID UnknownAddress OPTIONAL,
	IN PHEAP_INFO HeapInfo OPTIONAL 
	);

NTSYSAPI
PVOID
NTAPI
RtlAllocateHeap(
	IN HANDLE HeapHandle,
	IN ULONG AllocationFlags,
	IN ULONG NumberOfBytes
	);

NTSYSAPI
PVOID
NTAPI
RtlReAllocateHeap(
	IN HANDLE HeapHandle,
	IN ULONG AllocationFlags,
	IN PVOID MemoryBlock,
	IN ULONG NumberOfBytes
	);

NTSYSAPI
BOOLEAN
NTAPI
RtlFreeHeap(
    IN HANDLE HeapHandle,
    IN ULONG FreeingFlags,
    IN PVOID MemoryBlock
   );

NTSYSAPI
ULONG
NTAPI
RtlSizeHeap(
	IN HANDLE HeapHandle,
	IN ULONG AllocationFlags,
	IN PVOID MemoryBlock
	);

NTSYSAPI
PVOID   
NTAPI
RtlDestroyHeap(
	IN HANDLE HeapHandle
	);

NTSYSAPI
BOOLEAN
NTAPI
RtlValidateHeap(
	IN HANDLE HeapHandle,
	IN ULONG AccessFlags,
	IN PVOID MemoryBlock OPTIONAL
	);

NTSYSAPI
ULONG 
NTAPI
RtlGetProcessHeaps(
	IN ULONG NumberOfHeaps,
	OUT PHANDLE ProcessHeaps 
	);

NTSYSAPI
ULONG 
NTAPI
RtlCompactHeap(
	IN HANDLE HeapHandle,
	IN ULONG AccessFlags
	);

NTSYSAPI
BOOLEAN
NTAPI
RtlLockHeap(
	IN HANDLE HeapHandle
	);

NTSYSAPI
BOOLEAN
NTAPI
RtlUnlockHeap(
	IN HANDLE HeapHandle
	);

typedef struct _RTL_PROCESS_HEAP_ENTRY {
    PVOID lpData;
    ULONG cbData;
    UCHAR cbOverhead;
    UCHAR iRegionIndex;
    SHORT wFlags;
    union {
        struct {
            HANDLE hMem;
            ULONG dwReserved[ 3 ];
        } Block;
        struct {
            ULONG dwCommittedSize;
            ULONG dwUnCommittedSize;
            PVOID lpFirstBlock;
            PVOID lpLastBlock;
        } Region;
    };
} RTL_PROCESS_HEAP_ENTRY, *PRTL_PROCESS_HEAP_ENTRY;

#define RTL_PROCESS_HEAP_REGION					0x0002
#define RTL_PROCESS_HEAP_UNCOMMITTED_RANGE		0x0100
#define RTL_PROCESS_HEAP_ENTRY_BUSY				0x0001
#define RTL_PROCESS_HEAP_ENTRY_MOVEABLE			0x0200
#define RTL_PROCESS_HEAP_ENTRY_DDESHARE_BUSY    0x0400

NTSYSAPI
BOOLEAN
NTAPI
RtlWalkHeap(
    IN HANDLE HeapHandle,
    IN OUT PRTL_PROCESS_HEAP_ENTRY EntryInfo
    );

NTSYSAPI
ULONG 
NTAPI
RtlCreateTagHeap(	
	IN HANDLE HeapHandle,
	IN ULONG Flags,
	IN PCWSTR TagName,
	IN PCWSTR TagSubName
	);

#include <pshpack4.h>
typedef struct _RTL_HEAP_TAG_INFO {
	ULONG AllocCount;
	ULONG FreeCount;
	ULONG MemoryUsed;
} RTL_HEAP_TAG_INFO, *LPRTL_HEAP_TAG_INFO, *PRTL_HEAP_TAG_INFO;
#include <poppack.h>

NTSYSAPI
PCWSTR 
NTAPI
RtlQueryTagHeap(
	IN HANDLE HeapHandle,
	IN ULONG Flags,
	IN USHORT TagNumber,
	IN BOOLEAN ZeroInternalTagInfo,
	OUT PRTL_HEAP_TAG_INFO HeapTagInfo OPTIONAL 
	);

//Объект секц?

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

#define SECTION_QUERY       0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010
#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
                            SECTION_MAP_WRITE |      \
                            SECTION_MAP_READ |       \
                            SECTION_MAP_EXECUTE |    \
                            SECTION_EXTEND_SIZE)

NTSYSAPI
NTSTATUS 
NTAPI
NtCreateSection(
	OUT PHANDLE SectionHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN PLARGE_INTEGER MaximumSize OPTIONAL,
	IN ULONG Protect,
	IN ULONG Attributes,
	IN HANDLE FileHandle OPTIONAL 
	);

NTSYSAPI
NTSTATUS
NTAPI
NtOpenSection(
    OUT PHANDLE SectionHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

#define SEC_ALIGN_BASE_AND_OFFSET 0x40000000

NTSYSAPI
NTSTATUS
NTAPI
NtMapViewOfSection(
    IN HANDLE SectionHandle,
    IN HANDLE ProcessHandle,
    IN OUT PVOID *BaseAddress,
    IN ULONG ZeroBits,
    IN ULONG CommitSize,
    IN OUT PLARGE_INTEGER SectionOffset OPTIONAL,
    IN OUT PULONG ViewSize,
    IN SECTION_INHERIT InheritDisposition,
    IN ULONG AllocationType, 
    IN ULONG Protect 
    );

NTSYSAPI
NTSTATUS 
NTAPI
NtUnmapViewOfSection(
	IN HANDLE ProcessHandle,
	IN PVOID BaseAddress
	);

typedef enum _SECTIONINFOCLASS {
	SectionBasicInformation,
	SectionImageInformation
} SECTIONINFOCLASS;

typedef struct _SECTION_BASIC_INFORMATION {
	ULONG Reserved;
	ULONG Attributes;
	LARGE_INTEGER MaximumSize;
} SECTION_BASIC_INFORMATION, *PSECTION_BASIC_INFORMATION;

//Атрибуты секции

#define IMAGE_FILE_RELOCS_STRIPPED           0x0001  // Информац? ?перемещения?удален?из файл?
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  // Файл може?исполнять? (?? не?неразрешенны?внешни?ссылок).
#define IMAGE_FILE_LINE_NUMS_STRIPPED        0x0004  // Номера стро?удален?из файл?
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Локальны?символ?удален?из файл?
#define IMAGE_FILE_BYTES_REVERSED_LO         0x0080  // Байт??младше?машинном слов?расположен??обратном по?дк?
#define IMAGE_FILE_32BIT_MACHINE             0x0100  // Компьюте??32-битовыми словам?.
#define IMAGE_FILE_DEBUG_STRIPPED            0x0200  // Отладочн? информац? удален?из файл??.DBG файл
#define IMAGE_FILE_SYSTEM                    0x1000  // Системны?файл.
#define IMAGE_FILE_DLL                       0x2000  // Файл есть DLL.
#define IMAGE_FILE_BYTES_REVERSED_HI         0x8000  // Байт??старше?машинном слов?расположен??обратном по?дк?

// Обозначения целевого процессора

#define IMAGE_FILE_MACHINE_UNKNOWN           0
#define IMAGE_FILE_MACHINE_I386              0x14c   
#define IMAGE_FILE_MACHINE_R3000             0x162   // little endian, 0x160 big endian
#define IMAGE_FILE_MACHINE_R4000             0x166   
#define IMAGE_FILE_MACHINE_R10000            0x168   
#define IMAGE_FILE_MACHINE_ALPHA             0x184   
#define IMAGE_FILE_MACHINE_POWERPC           0x1F0   // little endian

// Обозначени?подсистемы

#define IMAGE_SUBSYSTEM_UNKNOWN              0   
#define IMAGE_SUBSYSTEM_NATIVE               1   
#define IMAGE_SUBSYSTEM_WINDOWS_GUI          2   
#define IMAGE_SUBSYSTEM_WINDOWS_CUI          3   
#define IMAGE_SUBSYSTEM_OS2_CUI              5   
#define IMAGE_SUBSYSTEM_POSIX_CUI            7   

typedef struct _SECTION_IMAGE_INFORMATION {
	ULONG EntryPoint;
	ULONG Unknown0;
	ULONG ReservedStackSize;
	ULONG CommittedStackSize;
	ULONG Subsystem;
	USHORT SubsystemVersionMinor;
	USHORT SubsystemVersionMajor;
	ULONG Unknown1;
	ULONG Characteristics;
	ULONG Machine;
	ULONG Unknown2;
	ULONG Unknown3;
	ULONG Unknown4;
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

NTSYSAPI
NTSTATUS
NTAPI
NtQuerySection(
	IN HANDLE SectionHandle,
	IN SECTIONINFOCLASS SectionInformationClass,
    OUT PVOID SectionInformation,
    IN ULONG SectionInformationLength,
    OUT PULONG ReturnLength OPTIONAL
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtExtendSection(
	IN HANDLE SectionHandle,
	IN PLARGE_INTEGER NewSize 
	);

//Объект процес?

typedef struct _CLIENT_ID {
    ULONG UniqueProcess;
    ULONG UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

#define NtCurrentProcess() ((HANDLE)-1)

typedef ULONG KAFFINITY;
typedef KAFFINITY *PKAFFINITY;
typedef LONG KPRIORITY;

//если флаг установлен, то вс?адреса абсолютные
#define PPFLAG_NORMALIZED 0x1L 

//
// Блок параметров процесса
//

typedef struct _PROCESS_PARAMETERS {
	ULONG AllocationSize;
	ULONG ActualSize;
	ULONG Flags;//PPFLAG_xxx
	ULONG Unknown1;
	ULONG Unknown2;
	ULONG Unknown3;
	HANDLE InputHandle;
	HANDLE OutputHandle;
	HANDLE ErrorHandle;
	UNICODE_STRING CurrentDirectory;
	HANDLE CurrentDir;
	UNICODE_STRING SearchPaths;
	UNICODE_STRING ApplicationName;
	UNICODE_STRING CommandLine;
	PVOID EnvironmentBlock;
	ULONG Unknown[9];
	UNICODE_STRING Unknown4;
	UNICODE_STRING Unknown5;
	UNICODE_STRING Unknown6;
	UNICODE_STRING Unknown7;
} PROCESS_PARAMETERS, *PPROCESS_PARAMETERS;

NTSYSAPI                                            
NTSTATUS
NTAPI
RtlCreateProcessParameters(
	OUT PPROCESS_PARAMETERS* ProcessParameters,
	IN PUNICODE_STRING ApplicationName,
	IN PUNICODE_STRING SearchPaths OPTIONAL,
	IN PUNICODE_STRING CurrentDirectory OPTIONAL,
	IN PUNICODE_STRING CommandLine OPTIONAL,
	IN PVOID EnvironmentBlock OPTIONAL,
	IN PUNICODE_STRING Unknown1 OPTIONAL,
	IN PUNICODE_STRING Unknown2 OPTIONAL,
	IN PUNICODE_STRING Unknown3 OPTIONAL,
	IN PUNICODE_STRING Unknown4 OPTIONAL
	);

NTSYSAPI                                            
PPROCESS_PARAMETERS 
NTAPI
RtlNormalizeProcessParams(
	IN PPROCESS_PARAMETERS ProcessParameters
	);

NTSYSAPI                                            
NTSTATUS
NTAPI
RtlDestroyProcessParameters(
	IN PPROCESS_PARAMETERS ProcessParameters
	);

NTSYSAPI                                            
PPROCESS_PARAMETERS
NTAPI
RtlDeNormalizeProcessParams(
	IN PPROCESS_PARAMETERS ProcessParameters
	);

NTSYSAPI                                            
PPROCESS_PARAMETERS
NTAPI
RtlNormalizeProcessParams(
	IN PPROCESS_PARAMETERS ProcessParameters
	);

typedef struct _RTL_PROCESS_INFORMATION {
	ULONG Size;
	//
	// PROCESS_INFORMATION
	//
	HANDLE ProcessHandle;
	HANDLE ThreadHandle;
	CLIENT_ID ClientId;
	//
	SECTION_IMAGE_INFORMATION SectionImageInfo;
} RTL_PROCESS_INFORMATION, *PRTL_PROCESS_INFORMATION;

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateUserProcess(
	IN PUNICODE_STRING FileName,
	IN ULONG FileObjectAttributes,
	IN PPROCESS_PARAMETERS ProcessParameters,
	IN PVOID ProcessSecurityDescriptor OPTIONAL,
	IN PVOID ThreadSecurityDescriptor OPTIONAL,
	IN HANDLE ParentProcess OPTIONAL,
	IN BOOLEAN InheritHandles,
	IN HANDLE DebugPort OPTIONAL,
	IN HANDLE ExceptionPort OPTIONAL,
	OUT PRTL_PROCESS_INFORMATION ProcessInfo
	);

//
// Блок окружения процесса
//

typedef struct _PEB {
	ULONG AllocationSize;
	ULONG Unknown1;
	HANDLE ProcessInstance;
	PVOID DllList;
	PPROCESS_PARAMETERS ProcessParameters;
	ULONG Unknown2;
	HANDLE DefaultHeap;

} PEB, *PPEB;


NTSYSAPI                                            
NTSTATUS                                            
NTAPI                                               
NtCreateProcess(
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN HANDLE ParentProcess,
	IN BOOLEAN InheritHandles,
	IN HANDLE SectionHandle,
	IN HANDLE DebugPort OPTIONAL,
	IN HANDLE ExceptionPort OPTIONAL
	);

NTSYSAPI
VOID
NTAPI
RtlAcquirePebLock(
	);

NTSYSAPI
VOID
NTAPI
RtlReleasePebLock(
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtTerminateProcess(
	IN HANDLE ProcessHandle,
	IN ULONG ProcessExitCode
	);

typedef struct _KERNEL_USER_TIMES {
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER ExitTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
} KERNEL_USER_TIMES;
typedef KERNEL_USER_TIMES *PKERNEL_USER_TIMES;

//Объект пото?

#define NtCurrentThread() ((HANDLE)-2)

NTSYSAPI
NTSTATUS 
NTAPI
RtlCreateUserThread(
	IN HANDLE ProcessHandle,
	IN PVOID SecurityDescriptor OPTIONAL,
	IN BOOLEAN CreateSuspended,
	IN ULONG ZeroBits OPTIONAL,
	IN ULONG ReservedStackSize OPTIONAL,
	IN ULONG CommittedStackSize OPTIONAL,
	IN PVOID StartRoutine,
	IN PVOID Argument,
	OUT PHANDLE ThreadHandle OPTIONAL,
	OUT PCLIENT_ID ClientID OPTIONAL
	);

#include <pshpack4.h>
typedef struct _STACK_INFO {
	ULONG Unknown1;
	ULONG Unknown2;
	PVOID StackBase;
	PVOID StackLimit;
	PVOID AllocationBase;
} STACK_INFO, *PSTACK_INFO;
#include <poppack.h>

// Структур?CONTEXT

#ifdef _M_IX86

#define SIZE_OF_80387_REGISTERS      80

// Флаг? опреде?ющие содержимое структур?CONTEXT.

#if !defined(RC_INVOKED)

#define CONTEXT_i386    0x00010000    // i386 ?i486 
#define CONTEXT_i486    0x00010000    // имею?одинаковые структур?контекст?

#define CONTEXT_CONTROL         (CONTEXT_i386 | 0x00000001L) // SS:SP, CS:IP, FLAGS, BP
#define CONTEXT_INTEGER         (CONTEXT_i386 | 0x00000002L) // AX, BX, CX, DX, SI, DI
#define CONTEXT_SEGMENTS        (CONTEXT_i386 | 0x00000004L) // DS, ES, FS, GS
#define CONTEXT_FLOATING_POINT  (CONTEXT_i386 | 0x00000008L) // сост?ни?387 
#define CONTEXT_DEBUG_REGISTERS (CONTEXT_i386 | 0x00000010L) // DB 0-3,6,7

#define CONTEXT_FULL (CONTEXT_CONTROL | CONTEXT_INTEGER |\
                      CONTEXT_SEGMENTS)

#endif

#ifndef _WINNT_

typedef struct _FLOATING_SAVE_AREA {
    ULONG   ControlWord;
    ULONG   StatusWord;
    ULONG   TagWord;
    ULONG   ErrorOffset;
    ULONG   ErrorSelector;	
    ULONG   DataOffset;
    ULONG   DataSelector;
    UCHAR   RegisterArea[SIZE_OF_80387_REGISTERS];
    ULONG   Cr0NpxState;	
} FLOATING_SAVE_AREA;

typedef FLOATING_SAVE_AREA *PFLOATING_SAVE_AREA;

typedef struct _CONTEXT {

    //Флаг? опреде?ющие содержимое структур?CONTEXT
	ULONG ContextFlags;

    // Эт?част?задает? ил?возвращает?,
	// если зада?флаг CONTEXT_DEBUG_REGISTERS ?ContextFlags.  

    ULONG   Dr0;
    ULONG   Dr1;
    ULONG   Dr2;
    ULONG   Dr3;
    ULONG   Dr6;
    ULONG   Dr7;

    // Эт?част?задает? ил?возвращает?,
	// если зада?флаг CONTEXT_FLOATING_POINT ?ContextFlags.  

    FLOATING_SAVE_AREA FloatSave;

    // Эт?част?задает? ил?возвращает?,
	// если зада?флаг CONTEXT_SEGMENTS ?ContextFlags.  

    ULONG   SegGs;
    ULONG   SegFs;	
    ULONG   SegEs;
    ULONG   SegDs;

    // Эт?част?задает? ил?возвращает?,
	// если зада?флаг CONTEXT_INTEGER ?ContextFlags.  

    ULONG   Edi;
    ULONG   Esi;
    ULONG   Ebx;
    ULONG   Edx;
    ULONG   Ecx;
    ULONG   Eax;

    // Эт?част?задает? ил?возвращает?,
	// если зада?флаг CONTEXT_CONTROL ?ContextFlags.  

    ULONG   Ebp;
    ULONG   Eip;
    ULONG   SegCs;              
    ULONG   EFlags;             
    ULONG   Esp;
    ULONG   SegSs;

} CONTEXT;

typedef CONTEXT *PCONTEXT;

#endif //_WINNT_

#endif // _M_IX86

NTSYSAPI
NTSTATUS 
NTAPI
NtCreateThread(
    OUT PHANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ProcessHandle,
	OUT PCLIENT_ID ClientID,
	IN PCONTEXT Context,
	IN PSTACK_INFO StackInfo,
	IN BOOLEAN CreateSuspended 
	);

NTSYSAPI
NTSTATUS
NTAPI
NtOpenThread(
    OUT PHANDLE ThreadHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PCLIENT_ID ClientId
    );

NTSYSAPI
NTSTATUS 
NTAPI
NtTerminateThread(
	IN HANDLE ThreadHandle,
	IN ULONG ThreadExitCode
	);

#if (_WIN32_WINNT >= 0x0400)
#define EXIT_STACK_SIZE 0x188
#else
#define EXIT_STACK_SIZE 0x190
#endif

//
// Блок окружения потока
//

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
	ULONG ExitStack[EXIT_STACK_SIZE];

} TEB;
typedef TEB *PTEB;

NTSYSAPI
PTEB
NTAPI
NtCurrentTeb(
	VOID
	);

typedef enum _THREADINFOCLASS {
    ThreadBasicInformation,//0
    ThreadTimes,//1
    ThreadPriority,//2
    ThreadBasePriority,//3
    ThreadAffinityMask,//4
    ThreadImpersonationToken,//5
    ThreadDescriptorTableEntry,//6
    ThreadEnableAlignmentFaultFixup,//7
    ThreadEventPair,//8
    ThreadQuerySetWin32StartAddress,//9
    ThreadZeroTlsCell,//0A(10)
    ThreadPerformanceCount,//0B(11)
    ThreadAmILastThread,//0C(12)
#if (_WIN32_WINNT >= 0x0400)
	ThreadIdealProcessor,//0D(13)
	ThreadPriorityBoost,//0E(14)
#endif //_WIN32_WINNT
    MaxThreadInfoClass
    } THREADINFOCLASS;

NTSYSAPI
NTSTATUS
NTAPI
NtQueryInformationThread(
    IN HANDLE ThreadHandle,
    IN THREADINFOCLASS ThreadInformationClass,
    OUT PVOID ThreadInformation,
    IN ULONG ThreadInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
NtSetInformationThread(
    IN HANDLE ThreadHandle,
    IN THREADINFOCLASS ThreadInformationClass,
    IN PVOID ThreadInformation,
    IN ULONG ThreadInformationLength
    );

typedef struct _THREAD_BASIC_INFORMATION {
	NTSTATUS ExitStatus;
	PTEB TebBaseAddress;
	CLIENT_ID ClientID;
	KAFFINITY AffinityMask;
	KPRIORITY BasePriority;
	KPRIORITY Priority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef struct _THREAD_AM_I_LAST_THREAD {
	BOOLEAN AmILastThread;
} THREAD_AM_I_LAST_THREAD, *PTHREAD_AM_I_LAST_THREAD;

NTSYSAPI
NTSTATUS
NTAPI
NtGetContextThread(
	IN HANDLE ThreadHandle,
	IN OUT PCONTEXT ThreadContext
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtSetContextThread(
	IN HANDLE ThreadHandle,
	IN PCONTEXT ThreadContext
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtResumeThread(
    IN PHANDLE ThreadHandle,
	OUT PULONG SuspendCount OPTIONAL
	);

NTSYSAPI
NTSTATUS 
NTAPI
NtSuspendThread(
    IN PHANDLE ThreadHandle,
	OUT PULONG SuspendCount OPTIONAL
	);

// Загрузчи?

NTSYSAPI
VOID 
NTAPI
LdrShutdownThread(
	VOID
	);

NTSYSAPI
VOID 
NTAPI
LdrShutdownProcess(
	VOID
	);

NTSYSAPI
NTSTATUS
NTAPI
LdrDisableThreadCalloutsForDll(
	IN HANDLE DllHandle
	);

NTSYSAPI
NTSTATUS
NTAPI
LdrUnloadDll(
	IN HANDLE DllHandle
	);

NTSYSAPI
NTSTATUS
NTAPI
LdrGetDllHandle(
	IN PCWSTR SearchPaths OPTIONAL,
	IN ULONG Reserved,	
	IN PUNICODE_STRING ModuleName,
	OUT PHANDLE DllHandle
	);

#define LDR_DONT_RESOLVE_DLL_REFERENCES	0x00000002L

NTSYSAPI
NTSTATUS
NTAPI
LdrLoadDll(
	IN PCWSTR SearchPaths OPTIONAL,
	IN PULONG ActionWhenLoading OPTIONAL,	
	IN PUNICODE_STRING ModuleName,
	OUT PHANDLE DllHandle
	);

NTSYSAPI
NTSTATUS
NTAPI
LdrGetProcedureAddress(
	IN HANDLE ModuleHandle,
	IN PSTRING ProcedureName OPTIONAL,
	IN ULONG ProcedureOrdinalValue OPTIONAL,
	OUT PVOID *ProcedureAddress
	);

NTSYSAPI
NTSTATUS
NTAPI
LdrVerifyImageMatchesChecksum(
	IN HANDLE FileHandle
	);

//SubKey under "\Registry\Machine\Software\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\"
NTSYSAPI
NTSTATUS
NTAPI
LdrQueryImageFileExecutionOptions(
	IN PUNICODE_STRING SubKey,
	IN PCWSTR ValueName,
	IN ULONG ValueSize,
	OUT PVOID Buffer,
	IN ULONG BufferSize,
	OUT PULONG ReturnedLength OPTIONAL
	);

#ifndef _WINNT_

#include <pshpack1.h>

#define IMAGE_NT_SIGNATURE                  0x00004550  // PE00

// Формат заголовк?файл?

typedef struct _IMAGE_FILE_HEADER {
    USHORT    Machine;
    USHORT    NumberOfSections;
    ULONG     TimeDateStamp;
    ULONG     PointerToSymbolTable;
    ULONG     NumberOfSymbols;
    USHORT    SizeOfOptionalHeader;
    USHORT    Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

#define IMAGE_SIZEOF_FILE_HEADER             20

// Формат каталога
typedef struct _IMAGE_DATA_DIRECTORY {
    ULONG   VirtualAddress;
    ULONG   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

// Формат необязательного заголовк?
typedef struct _IMAGE_OPTIONAL_HEADER {

	//Стандартны?по?
    USHORT    Magic;
    UCHAR    MajorLinkerVersion;
    UCHAR    MinorLinkerVersion;
    ULONG   SizeOfCode;
    ULONG   SizeOfInitializedData;
    ULONG   SizeOfUninitializedData;
    ULONG   AddressOfEntryPoint;
    ULONG   BaseOfCode;
    ULONG   BaseOfData;

    // Дополнительные по? NT
    ULONG   ImageBase;
    ULONG   SectionAlignment;
    ULONG   FileAlignment;
    USHORT    MajorOperatingSystemVersion;
    USHORT    MinorOperatingSystemVersion;
    USHORT    MajorImageVersion;
    USHORT    MinorImageVersion;
    USHORT    MajorSubsystemVersion;
    USHORT    MinorSubsystemVersion;
    ULONG   Reserved1;
    ULONG   SizeOfImage;
    ULONG   SizeOfHeaders;
    ULONG   CheckSum;
    USHORT    Subsystem;
    USHORT    DllCharacteristics;
    ULONG   SizeOfStackReserve;
    ULONG   SizeOfStackCommit;
    ULONG   SizeOfHeapReserve;
    ULONG   SizeOfHeapCommit;
    ULONG   LoaderFlags;
    ULONG   NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

#define IMAGE_SIZEOF_STD_OPTIONAL_HEADER      28
#define IMAGE_SIZEOF_NT_OPTIONAL_HEADER      224

#define IMAGE_NT_OPTIONAL_HDR_MAGIC        0x10b

typedef struct _IMAGE_NT_HEADERS {
    ULONG Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER OptionalHeader;
} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;

#include <poppack.h>

#endif //_WINNT_

NTSYSAPI                                            
PIMAGE_NT_HEADERS
NTAPI                                               
RtlImageNtHeader(
	IN HANDLE InstanceHandle 
	);

// Блок переменных окружения

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateEnvironment(
	IN BOOLEAN CopyCurrent,
	OUT PVOID* EnvironmentBlock
	);

NTSYSAPI                                            
NTSTATUS
NTAPI                                               
RtlDestroyEnvironment(
	IN PVOID EnvironmentBlock
	);

NTSYSAPI
NTSTATUS
NTAPI
RtlExpandEnvironmentStrings_U(
	IN PVOID EnvironmentBlock OPTIONAL,
	IN PUNICODE_STRING SourceString,
	OUT PUNICODE_STRING ExpandedString,
	OUT PULONG BytesRequired
	);

NTSYSAPI
NTSTATUS
NTAPI
RtlSetEnvironmentVariable(
	IN PVOID EnvironmentBlock OPTIONAL,
	IN PUNICODE_STRING VariableName,
	IN PUNICODE_STRING VariableValue OPTIONAL
	);

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryEnvironmentVariable_U(
	IN PVOID EnvironmentBlock OPTIONAL,
	IN PUNICODE_STRING VariableName,
	OUT PUNICODE_STRING VariableValue
	);

NTSYSAPI
NTSTATUS
NTAPI
RtlSetCurrentEnvironment(
	PVOID NewEnvironmentBlock,
	PVOID* OldEnvironmentBlock OPTIONAL
	);

// Вспомогательная функция для BootExecute-программ

NTSYSAPI
VOID 
NTAPI
NtDisplayString(
	IN PUNICODE_STRING DisplayString
	);

//Полезные макрос?

#define NtGetProcessHeap()		(NtCurrentTeb()->PebBaseAddress->DefaultHeap)

//Реестр ?объект ключ реестр?

typedef NTSTATUS (*PRTL_QUERY_REGISTRY_ROUTINE)(
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength,
    IN PVOID Context,
    IN PVOID EntryContext
    );

typedef struct _RTL_QUERY_REGISTRY_TABLE {
    PRTL_QUERY_REGISTRY_ROUTINE QueryRoutine;
    ULONG Flags;
    PWSTR Name;
    PVOID EntryContext;
    ULONG DefaultType;
    PVOID DefaultData;
    ULONG DefaultLength;

} RTL_QUERY_REGISTRY_TABLE, *PRTL_QUERY_REGISTRY_TABLE;


//
// The following flags specify how the Name field of a RTL_QUERY_REGISTRY_TABLE
// entry is interpreted.  A NULL name indicates the end of the table.
//

#define RTL_QUERY_REGISTRY_SUBKEY   0x00000001  // Name is a subkey and remainder of
                                                // table or until next subkey are value
                                                // names for that subkey to look at.

#define RTL_QUERY_REGISTRY_TOPKEY   0x00000002  // Reset current key to original key for
                                                // this and all following table entries.

#define RTL_QUERY_REGISTRY_REQUIRED 0x00000004  // Fail if no match found for this table
                                                // entry.

#define RTL_QUERY_REGISTRY_NOVALUE  0x00000008  // Used to mark a table entry that has no
                                                // value name, just wants a call out, not
                                                // an enumeration of all values.

#define RTL_QUERY_REGISTRY_NOEXPAND 0x00000010  // Used to suppress the expansion of
                                                // REG_MULTI_SZ into multiple callouts or
                                                // to prevent the expansion of environment
                                                // variable values in REG_EXPAND_SZ

#define RTL_QUERY_REGISTRY_DIRECT   0x00000020  // QueryRoutine field ignored.  EntryContext
                                                // field points to location to store value.
                                                // For null terminated strings, EntryContext
                                                // points to UNICODE_STRING structure that
                                                // that describes maximum size of buffer.
                                                // If .Buffer field is NULL then a buffer is
                                                // allocated.
                                                //

#define RTL_QUERY_REGISTRY_DELETE   0x00000040  // Used to delete value keys after they
                                                // are queried.

NTSYSAPI
NTSTATUS
NTAPI
RtlQueryRegistryValues(
    IN ULONG RelativeTo,
    IN PWSTR Path,
    IN PRTL_QUERY_REGISTRY_TABLE QueryTable,
    IN PVOID Context,
    IN PVOID Environment OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlWriteRegistryValue(
    IN ULONG RelativeTo,
    IN PWSTR Path,
    IN PWSTR ValueName,
    IN ULONG ValueType,
    IN PVOID ValueData,
    IN ULONG ValueLength
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlDeleteRegistryValue(
    IN ULONG RelativeTo,
    IN PWSTR Path,
    IN PWSTR ValueName
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCreateRegistryKey(
    IN ULONG RelativeTo,
    IN PWSTR Path
    );

NTSYSAPI
NTSTATUS
NTAPI
RtlCheckRegistryKey(
    IN ULONG RelativeTo,
    IN PWSTR Path
    );

//
// The following values for the RelativeTo parameter determine what the
// Path parameter to RtlQueryRegistryValues is relative to.
//

#define RTL_REGISTRY_ABSOLUTE     0   // Path is a full path
#define RTL_REGISTRY_SERVICES     1   // \Registry\Machine\System\CurrentControlSet\Services
#define RTL_REGISTRY_CONTROL      2   // \Registry\Machine\System\CurrentControlSet\Control
#define RTL_REGISTRY_WINDOWS_NT   3   // \Registry\Machine\Software\Microsoft\Windows NT\CurrentVersion
#define RTL_REGISTRY_DEVICEMAP    4   // \Registry\Machine\Hardware\DeviceMap
#define RTL_REGISTRY_USER         5   // \Registry\User\CurrentUser
#define RTL_REGISTRY_MAXIMUM      6
#define RTL_REGISTRY_HANDLE       0x40000000    // Low order bits are registry handle
#define RTL_REGISTRY_OPTIONAL     0x80000000    // Indicates the key node is optional

//
// Registry Specific Access Rights.
//

#define KEY_QUERY_VALUE         (0x0001)
#define KEY_SET_VALUE           (0x0002)
#define KEY_CREATE_SUB_KEY      (0x0004)
#define KEY_ENUMERATE_SUB_KEYS  (0x0008)
#define KEY_NOTIFY              (0x0010)
#define KEY_CREATE_LINK         (0x0020)

#define KEY_READ                ((STANDARD_RIGHTS_READ       |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY)                 \
                                  &                           \
                                 (~SYNCHRONIZE))


#define KEY_WRITE               ((STANDARD_RIGHTS_WRITE      |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY)         \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_EXECUTE             ((KEY_READ)                   \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_ALL_ACCESS          ((STANDARD_RIGHTS_ALL        |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY         |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY                 |\
                                  KEY_CREATE_LINK)            \
                                  &                           \
                                 (~SYNCHRONIZE))

//
// Open/Create Options
//

#define REG_OPTION_RESERVED         (0x00000000L)   // Parameter is reserved

#define REG_OPTION_NON_VOLATILE     (0x00000000L)   // Key is preserved
                                                    // when system is rebooted

#define REG_OPTION_VOLATILE         (0x00000001L)   // Key is not preserved
                                                    // when system is rebooted

#define REG_OPTION_CREATE_LINK      (0x00000002L)   // Created key is a
                                                    // symbolic link

#define REG_OPTION_BACKUP_RESTORE   (0x00000004L)   // open for backup or restore
                                                    // special access rules
                                                    // privilege required

#define REG_OPTION_OPEN_LINK        (0x00000008L)   // Open symbolic link

#ifndef REG_LEGAL_OPTION
#define REG_LEGAL_OPTION            \
                (REG_OPTION_RESERVED            |\
                 REG_OPTION_NON_VOLATILE        |\
                 REG_OPTION_VOLATILE            |\
                 REG_OPTION_CREATE_LINK         |\
				 REG_OPTION_BACKUP_RESTORE		|\
                 REG_OPTION_OPEN_LINK)
#endif

//
// Key creation/open disposition
//

#define REG_CREATED_NEW_KEY         (0x00000001L)   // New Registry Key created
#define REG_OPENED_EXISTING_KEY     (0x00000002L)   // Existing Key opened

//
// Key restore flags
//

#define REG_WHOLE_HIVE_VOLATILE     (0x00000001L)   // Restore whole hive volatile
#define REG_REFRESH_HIVE            (0x00000002L)   // Unwind changes to last flush
#define REG_NO_LAZY_FLUSH           (0x00000004L)   // Never lazy flush this hive

//
// Key query structures
//

typedef struct _KEY_BASIC_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

typedef struct _KEY_NODE_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
//          Class[1];           // Variable length string not declared
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

typedef struct _KEY_FULL_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   SubKeys;
    ULONG   MaxNameLen;
    ULONG   MaxClassLen;
    ULONG   Values;
    ULONG   MaxValueNameLen;
    ULONG   MaxValueDataLen;
    WCHAR   Class[1];           // Variable length
} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION;

typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation
} KEY_INFORMATION_CLASS;

typedef struct _KEY_WRITE_TIME_INFORMATION {
    LARGE_INTEGER LastWriteTime;
} KEY_WRITE_TIME_INFORMATION, *PKEY_WRITE_TIME_INFORMATION;

typedef enum _KEY_SET_INFORMATION_CLASS {
    KeyWriteTimeInformation
} KEY_SET_INFORMATION_CLASS;

//
// Value entry query structures
//

typedef struct _KEY_VALUE_BASIC_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataOffset;
    ULONG   DataLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation, 
    KeyValuePartialInformation 
} KEY_VALUE_INFORMATION_CLASS;

//
// Predefined Value Types.
//

#define REG_NONE                    ( 0 )   // No value type
#define REG_SZ                      ( 1 )   // Unicode nul terminated string
#define REG_EXPAND_SZ               ( 2 )   // Unicode nul terminated string
                                            // (with environment variable references)
#define REG_BINARY                  ( 3 )   // Free form binary
#define REG_DWORD                   ( 4 )   // 32-bit number
#define REG_DWORD_LITTLE_ENDIAN     ( 4 )   // 32-bit number (same as REG_DWORD)
#define REG_DWORD_BIG_ENDIAN        ( 5 )   // 32-bit number
#define REG_LINK                    ( 6 )   // Symbolic Link (unicode)
#define REG_MULTI_SZ                ( 7 )   // Multiple Unicode strings
#define REG_RESOURCE_LIST           ( 8 )   // Resource list in the resource map
#define REG_FULL_RESOURCE_DESCRIPTOR ( 9 )  // Resource list in the hardware description
#define REG_RESOURCE_REQUIREMENTS_LIST ( 10 )

NTSYSAPI
NTSTATUS
NTAPI
NtCreateKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG TitleIndex,
    IN PUNICODE_STRING Class OPTIONAL,
    IN ULONG CreateOptions,
    OUT PULONG Disposition OPTIONAL
    );

NTSYSAPI
NTSTATUS
NTAPI
NtOpenKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSAPI
NTSTATUS
NTAPI
NtDeleteKey(
    IN HANDLE KeyHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
NtEnumerateKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
NtEnumerateValueKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
NtFlushKey(
    IN HANDLE KeyHandle
    );

NTSYSAPI
NTSTATUS
NTAPI
NtQueryKey(
    IN HANDLE KeyHandle,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
NtQueryValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSAPI
NTSTATUS
NTAPI
NtSetValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN ULONG TitleIndex OPTIONAL,
    IN ULONG Type,
    IN PVOID Data,
    IN ULONG DataSize
    );

NTSYSAPI
NTSTATUS
NTAPI
NtDeleteValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName
	);

NTSYSAPI
NTSTATUS
NTAPI
NtLoadKey(
	//directory field specified parent key
	//name field specified creating key
	IN POBJECT_ATTRIBUTES KeyToLoad,
	IN POBJECT_ATTRIBUTES File
	);

NTSYSAPI
NTSTATUS
NTAPI
NtUnloadKey(
	//directory field specified parent key
	//name field specified unloading key
	IN POBJECT_ATTRIBUTES KeyToUnLoad
	);

NTSYSAPI
NTSTATUS
NTAPI
NtSaveKey(
	IN HANDLE KeyHandle,
	IN HANDLE FileHandle
	);

NTSYSAPI
NTSTATUS
NTAPI
NtRestoreKey(
	IN HANDLE KeyHandle,
	IN HANDLE FileHandle,
	IN ULONG Flags
	);

NTSYSAPI
NTSTATUS
NTAPI
NtReplaceKey(
	IN POBJECT_ATTRIBUTES NewFile,
	IN HANDLE KeyHandle,
	IN POBJECT_ATTRIBUTES OldFile
	);

// Безопасность

#define SE_MIN_WELL_KNOWN_PRIVILEGE       (2L)
#define SE_CREATE_TOKEN_PRIVILEGE         (2L)
#define SE_ASSIGNPRIMARYTOKEN_PRIVILEGE   (3L)
#define SE_LOCK_MEMORY_PRIVILEGE          (4L)
#define SE_INCREASE_QUOTA_PRIVILEGE       (5L)

//
// Unsolicited Input is obsolete and unused.
//

#define SE_UNSOLICITED_INPUT_PRIVILEGE    (6L)

#define SE_MACHINE_ACCOUNT_PRIVILEGE      (6L)
#define SE_TCB_PRIVILEGE                  (7L)
#define SE_SECURITY_PRIVILEGE             (8L)
#define SE_TAKE_OWNERSHIP_PRIVILEGE       (9L)
#define SE_LOAD_DRIVER_PRIVILEGE          (10L)
#define SE_SYSTEM_PROFILE_PRIVILEGE       (11L)
#define SE_SYSTEMTIME_PRIVILEGE           (12L)
#define SE_PROF_SINGLE_PROCESS_PRIVILEGE  (13L)
#define SE_INC_BASE_PRIORITY_PRIVILEGE    (14L)
#define SE_CREATE_PAGEFILE_PRIVILEGE      (15L)
#define SE_CREATE_PERMANENT_PRIVILEGE     (16L)
#define SE_BACKUP_PRIVILEGE               (17L)
#define SE_RESTORE_PRIVILEGE              (18L)
#define SE_SHUTDOWN_PRIVILEGE             (19L)
#define SE_DEBUG_PRIVILEGE                (20L)
#define SE_AUDIT_PRIVILEGE                (21L)
#define SE_SYSTEM_ENVIRONMENT_PRIVILEGE   (22L)
#define SE_CHANGE_NOTIFY_PRIVILEGE        (23L)
#define SE_REMOTE_SHUTDOWN_PRIVILEGE      (24L)
#define SE_MAX_WELL_KNOWN_PRIVILEGE       (SE_REMOTE_SHUTDOWN_PRIVILEGE)

NTSYSAPI
NTSTATUS
NTAPI
RtlAdjustPrivilege(
	IN ULONG Privilege,//SE_xxx_PRIVILEGE
	IN BOOLEAN Enable,
	IN BOOLEAN CurrentThread,//FALSE-CurrentProcess
	OUT PBOOLEAN Enabled 
	);

// ?библиотека времен?выполнен?

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifndef _CRTIMP
#define _CRTIMP NTSYSAPI 
#endif 

#ifndef _INC_STRING
#ifndef _INC_MEMORY
_CRTIMP void *  __cdecl memmove(void *, const void *, size_t);
#endif //_INC_MEMORY
#endif //_INC_STRING

#ifndef _INC_STDIO
#ifndef _INC_WCHAR
_CRTIMP int __cdecl swprintf(wchar_t *, const wchar_t *, ...);
#endif //_INC_WCHAR
#endif //_INC_STDIO

//////////////////////////////////////////////////////////////////////////
// Added by Xiao

NTSYSAPI
NTSTATUS
NTAPI
ZwSuspendProcess (
				  HANDLE ProcessHandle
				  );

NTSYSAPI
NTSTATUS
NTAPI
ZwResumeProcess (
				 HANDLE ProcessHandle
				 );

#ifdef __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  // _MSC_VER 

#endif //_NTDLL_H
