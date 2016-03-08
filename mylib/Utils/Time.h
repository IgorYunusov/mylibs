#pragma once

#ifdef int64
#undef int64
#endif

#include <atltime.h>

#ifndef int64
#define int64	__int64
#endif

namespace Utils {

	/*
	class __InitUtils {
	public:
		__InitUtils()
		{
			
		}

		static unsigned long startTick;
	};

	*/

	inline unsigned long getMsTime()
	{
		return GetTickCount();
	}

	typedef CTime Time;
	typedef CTimeSpan TimeSpan;
};
