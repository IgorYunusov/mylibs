#include "mylib.h"
#include "Scheduler.h"
#include "macros.h"

// no implementation

#if 0
ScheItem::ScheItem(ScheTime& atime, ULONG period, ULONG times): 
	_atime(atime), _period(period), _times(times)
{
	_enabled = true;
	_timer = NULL;
	_count = 0;
}

inline void ScheItem::init()
{
	if (!_enabled)
		return;

	if (_count == 0 || _times != 0xffffffff)
		_count ++;

	if (_count > _times)
		return;

	ScheTimeSpan span;
	ScheTime now = GetScheTimeNow();
	span = _atime - now;
	LONGLONG r = span.GetTotalSeconds();

	if (r < 0) {
		r = _period - ((-r) % _period);			
	}

	if (_count == 1) {
		_atime = now;
        _atime += (r + _period);
	} else {
		_atime += _period;
	}

	if (_timer == NULL) {
		_timer = new ScheTimer(this, (ULONG )r);
		_scheduler->pushTimer(_timer);
	} else if (_timer->getinterval() != r * 1000) {
		_scheduler->removeTimer(_timer);
		_timer = new ScheTimer(this, (ULONG )r);
		_scheduler->pushTimer(_timer);
	}
}

inline void ScheItem::final()
{

}

void ScheItem::_process()
{	
	process();
	final();

	if (_count < _times && _enabled) {
		init();

	} else {

		if (_timer) {
			_scheduler->removeTimer(_timer);
			_timer = NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

Scheduler::Scheduler()
{

}

Scheduler::~Scheduler(void)
{

}

ULONG Scheduler::push(ScheItem* item)
{
	ScheItemList::iterator it = _list.insert(_list.begin(), item);
	(*it)->_scheduler = this;
	(*it)->init();	
	return CastLong(&*it);
}

bool Scheduler::enable(ULONG id, bool enabled)
{
	ScheItem* item = *((ScheItem** )CastPtr(id));
	if (IsBadWritePtr(item, sizeof(item)))
		return false;

	item->_enabled = enabled;
	return true;
}
#endif
