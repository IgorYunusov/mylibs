#pragma once

#ifndef __countof
#define __countof(a)		(sizeof( a ) / sizeof( (a)[0] ))
#endif

#ifndef MakePtr
#define MakePtr(b, o)		(PVOID )((ULONG_PTR )(b) + (ULONG_PTR )(o))
#endif

#ifndef CastLong
#define CastLong(P)			( (ULONG )( (PBYTE )(P) - (PBYTE )NULL ) )
#endif

#ifndef CastPtr
#define CastPtr(L)			PBYTE( (PBYTE )0  + (ULONG_PTR )L )
#endif

template<typename T1, typename T2>
void Cast(T1& d, T2 s)
{
	union {
		T1 a;
		T2 b;
	} u;

	u.b = s;
	d = u.a;
}

template<typename T1, typename T2>
T1 Cast(T2 s)
{
	union {
		T1 a;
		T2 b;
	} u;

	u.b = s;
	return u.a;
}
