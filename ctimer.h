
// -----------------------------------------------------------------------------------------------------------------------
//  Class:  CTimer
//
//  Modification History :
//  Date                Author                                Comment
//  ---------------    -----------    -----------------------------------------------------------
//  2002.10.18
// -----------------------------------------------------------------------------------------------------------------------

#ifndef _CTIMER
#define _CTIMER

#include <sys/types.h>
#ifdef _TSMC_UNIX
   #include <stdlib.h>
   #include <stdio.h>
#else
   #include <sys/timeb.h>
#endif
#include <map>
#include <string>
#include <iostream>


class CExecTimer
{
public:
	CExecTimer();
	void                         start();
	void                         end();
	void						 reset();
	double                       ConsumeTime() const;
	int							 Count() const;
	void	                     print(std::ostream &os = std::cout) const;
private:

#ifdef _TSMC_UNIX
	time_t                       _start_time;
#else
	struct _timeb                _start_time;
#endif

	int                          _count = 0;
	double                       _consumed_time = 0.;
};


class CExecCounter
{
public:
	CExecCounter():_count(0) { }
	CExecCounter & operator ++ () { _count++; return *this; }
	int                          Count() const  { return _count; }
	void						 reset() { _count = 0; }
	void                         print(std::ostream &os = std::cout) const;
private:
	int                           _count;
};

class CExecTimerGroup
{
public:
	CExecTimer *                 GetTimer(const std::string &name);
	const CExecTimer *           GetTimer(const std::string &name) const;

	CExecCounter *               GetCounter(const std::string &name);
	const CExecCounter *         GetCounter(const std::string &name) const;

	void                         print(std::ostream &os = std::cout) const;
private:

	typedef std::map<std::string, CExecTimer>     TETMap;
	typedef TETMap::iterator            TETMapIterator;
	typedef TETMap::const_iterator      TETMapConstIterator;
	std::map<std::string, CExecTimer>             _timer_group;

	typedef std::map<std::string, CExecCounter>   TECMap;
	typedef TECMap::iterator            TECMapIterator;
	typedef TECMap::const_iterator      TECMapConstIterator;
	std::map<std::string, CExecCounter>           _counter_group;
};

extern CExecTimerGroup MyTimers;

#endif
