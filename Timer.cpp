#include "pch.h"
#include "Timer.h"


Timer::Timer(void)
{
	stop = true;
	timeOut = -1;
	threadDone= NULL;
	threadDone = CreateEvent(NULL, FALSE, FALSE, NULL);
	ASSERT(threadDone);
	SetEvent(threadDone);
}


Timer::~Timer(void)
{
	DWORD ret = WaitForSingleObject(threadDone, INFINITE);
	ASSERT(ret == WAIT_OBJECT_0);
	Sleep(100);
}


void Timer::Timeout()
{
}


void Timer::startTimer(void)
{
	if (stop == false) return;
	stop = false;
	ResetEvent(threadDone);
	AfxBeginThread(tickerThread, this);
}


UINT Timer::tickerThread(LPVOID pParam)
{
	Timer* me = (Timer*)pParam;
	ASSERT(me->timeOut != -1);
	while (!me->stop)
	{
		Sleep(me->getTimeout());
		me->Timeout();
	}
	SetEvent(me->threadDone);
	return 0;
}


void Timer::stopTimer(void)
{
	if (stop == true) return;
	stop = true;
	WaitForSingleObject(threadDone, INFINITE);
}


int Timer::getTimeout(void)
{
	return timeOut;
}


void Timer::setTimeout(int t)
{
	timeOut = t;
}
