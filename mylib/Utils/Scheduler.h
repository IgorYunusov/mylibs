#pragma once

// no implementation

#if 0

#include "timer.h"
#include <atltime.h>

#define SECONDS_PER_HOUR		(60 * 60)
#define SECONDS_PER_MINUTE		60

class Scheduler;

typedef ATL::CTime ScheTime;

inline ScheTime GetScheTimeNow()
{
	return ScheTime(_time64(NULL));
}

typedef ATL::CTimeSpan ScheTimeSpan;

class ScheItem {
	friend class Scheduler;
public:
	ScheItem(ScheTime& atime, ULONG period, ULONG times);

protected:
	
	void _process();

	class ScheTimer: public ExactTimer {
	public:
		ScheTimer(ScheItem* item, ULONG interval): _thisPtr(item)
		{
			beginTimer(interval);
		}

		virtual void onTimer()
		{
			_thisPtr->onTimer();
		}

	protected:
		ScheItem*	_thisPtr;
	};

	void init();
	void final();

	virtual void onTimer() { }

protected:
	ScheTime	_atime;	// 下次激活时间
	ULONG		_period;
	ULONG		_times;
	ULONG		_count;
	bool		_enabled;
	ScheTimer*	_timer;
	Scheduler*	_scheduler;
};

class Scheduler
{
	friend class ScheItem;

public:
	Scheduler();
	~Scheduler(void);

	ULONG push(ScheItem* item);
	bool enable(ULONG id, bool enabled);

protected:

protected:
	typedef std::list<ScheItem* > ScheItemList;

	ScheItemList		_list;
};
#endif
