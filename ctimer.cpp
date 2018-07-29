
// -----------------------------------------------------------------------------------------------------------------------
//  Class:  CTimer
//
//  Modification History :
//  Date                Author                                Comment
//  ---------------    -----------    -----------------------------------------------------------
//  2002.10.18     YaKai
// -----------------------------------------------------------------------------------------------------------------------

#include "CTimer.h"

#include <iostream>
#include <utility>

using namespace std;

CExecTimerGroup MyTimers;

// -----------------------------------------------------------------------------------------------------------------------
//										CExecTimer
// -----------------------------------------------------------------------------------------------------------------------
CExecTimer::CExecTimer()
{
#ifdef _TSMC_UNIX
	_start_time = -1;
#else
	_start_time.time = -1;
#endif
}
// -----------------------------------------------------------------------------------------------------------------------
void CExecTimer::start()
{
#ifdef _TSMC_UNIX
	time(&_start_time);
#else
	_ftime64_s(&_start_time);
#endif
}
// -----------------------------------------------------------------------------------------------------------------------
void CExecTimer::end()
{
#ifdef _TSMC_UNIX
	if (_start_time != -1)
	{
		time_t now;
		time(&now);
	        _consumed_time += difftime(now,_start_time);
		_count ++;

		_start_time = -1;
	}
#else
	if (_start_time.time != -1)
	{
		struct _timeb now;
		_ftime64_s(&now);

		_consumed_time += now.time - _start_time.time;
		_consumed_time += (now.millitm - _start_time.millitm) * 0.001;

		_count ++;

		_start_time.time = -1;
	}
#endif
}
// -----------------------------------------------------------------------------------------------------------------------
void CExecTimer::reset()
{
	_count = 0;
	_consumed_time = 0;
}
// -----------------------------------------------------------------------------------------------------------------------
double CExecTimer::ConsumeTime() const
{
	return _consumed_time;
}
// -----------------------------------------------------------------------------------------------------------------------
int	CExecTimer::Count() const
{
	return _count;
}
// -----------------------------------------------------------------------------------------------------------------------
void CExecTimer::print(ostream &os) const
{
	os << "Count : " << _count << " Consumed time : " << _consumed_time << " seconds." << endl;
}
// -----------------------------------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------------------------------------
//										CExecCounter
// -----------------------------------------------------------------------------------------------------------------------
void CExecCounter::print(ostream &os) const
{
	os << "Count : " << _count << endl;
}


// -----------------------------------------------------------------------------------------------------------------------
//										CExecTimerGroup
// -----------------------------------------------------------------------------------------------------------------------

CExecTimer * CExecTimerGroup::GetTimer(const string &name)
{
	TETMapIterator it;

	it = _timer_group.find(name);

	if (it == _timer_group.end())
	{
		_timer_group.insert(make_pair(name,CExecTimer()));
		it = _timer_group.find(name);
	}
	return &(it->second);
}
// -----------------------------------------------------------------------------------------------------------------------
const CExecTimer * CExecTimerGroup::GetTimer(const string &name) const
{
	TETMapConstIterator it;

	it = _timer_group.find(name);

	if (it == _timer_group.end())
		return NULL;
	else
		return &(it->second);
}
// -----------------------------------------------------------------------------------------------------------------------
CExecCounter * CExecTimerGroup::GetCounter(const string &name)
{
	TECMapIterator it;

	it = _counter_group.find(name);

	if (it == _counter_group.end())
	{
		_counter_group.insert(make_pair(name,CExecCounter()));
		it = _counter_group.find(name);
	}
	return &(it->second);
}
// -----------------------------------------------------------------------------------------------------------------------
const CExecCounter * CExecTimerGroup::GetCounter(const string &name) const
{
	TECMapConstIterator it;

	it = _counter_group.find(name);

	if (it == _counter_group.end())
		return NULL;
	else
		return &(it->second);
}
// -----------------------------------------------------------------------------------------------------------------------
void CExecTimerGroup::print(ostream &os) const
{
	TETMapConstIterator it, it_end;

	it = _timer_group.begin();
	it_end = _timer_group.end();

	os << endl << " .................. Timer statistics ............." << endl;
	while (it != it_end)
	{
		os << it->first << " - ";
		it->second.print(os);
		it ++;
	}

	TECMapConstIterator it2, it2_end;
	it2 = _counter_group.begin();
	it2_end = _counter_group.end();

	os << endl << " .................. Counter statistics ............." << endl;
	while (it2 != it2_end)
	{
		os << it2->first << " - ";
		it2->second.print(os);
		it2 ++;
	}

}
// -----------------------------------------------------------------------------------------------------------------------
