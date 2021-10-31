#pragma once

class Timer
{
public:
	Timer(void);
	~Timer(void);
	void startTimer(void);
	void stopTimer(void);
	int getTimeout(void);
	void setTimeout(int t);
protected:
	int timeOut;
	virtual void Timeout();
private:
	HANDLE threadDone;
	bool stop;
	static UINT tickerThread(LPVOID pParam);
};

