.386
.model flat, stdcall

public RpcStubLen
public RpcStubCode
public RpcStubProc
public RpcStubParams
public RpcStubConv
public RpcStubParamCount
public RpcStubSetThisPtr
public RpcStubExitThread

; ExitThread proto stdcall :DWORD;

.const

RpcStubLen		dd _RpcStubLen
RpcStubCode		dd RpcStub
RpcStubProc		dd _RpcStubProc
RpcStubParams	dd _RpcStubParams
RpcStubConv		dd _RpcStubConv
RpcStubParamCount	dd _RpcStubParamCount
RpcStubSetThisPtr dd _RpcStubSetThisPtr
RpcStubExitThread	dd _RpcStubExitThread

.code

RpcStub proc uses esi edi ebx p0 :DWORD
	
	; install seh
	call	@f
@@:
	pop		eax
	add		eax, RpcStubSehHandler - $ + 1; 1 byte code of pop eax
	assume fs:nothing
	push	eax
	push	fs:[0]
	mov		fs:[0], esp
	
	; copy param
_RpcStubParams = $ - RpcStub + 1
	mov		esi, 87654321h	; params address	
_RpcStubParamCount = $ - RpcStub + 1
	mov		ecx, 00000001h	; param count	
	mov		ebx, ecx
	shl		ebx, 2
	sub		esp, ebx		; alloc param space	
	mov		edi, esp
	rep		movsd			; copy param

	; call the procedure
_RpcStubProc = $ - RpcStub + 1
	mov		eax, 87654321h	; 调用地址
_RpcStubSetThisPtr = $ - RpcStub + 1
	mov		ecx, 87654321h	; this pointer
	call	eax				; 不要再碰 eax

	; leave the procedure
_RpcStubConv = $ - 	RpcStub + 1
	mov		edx, 00000001h	
	cmp		edx, 00000000h
	jnz		@F
	add		esp, ebx		; cdecl
	
@@:	
	; uninstall seh
	pop		fs:[0]
	pop		ecx
		
	ret
	
RpcStub endp

RpcStubSehHandler proc
	push	0c0000005h
_RpcStubExitThread = $ - RpcStub + 1
	mov		eax, 87654321h
	call	eax
	retn 10h	
RpcStubSehHandler endp

_RpcStubLen = $ - RpcStub

end
