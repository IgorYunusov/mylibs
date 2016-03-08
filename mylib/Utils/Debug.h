#pragma once

void MyTrace(LPCSTR fmt, ...);

#ifndef _DEBUG
#define _NO_TRACE
#endif

#ifndef _NO_TRACE
#define _TRACE	MyTrace
#else
#define _TRACE		
#endif
