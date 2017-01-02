#pragma once
class Time : public Singleton<Time>
{
	friend int main();
public:
	Time();
	~Time();

public:
	//프로그램 시작시간으로부터 흐른 시간(초)
	double GetNowTimeSinceStart();
	Get(double, DeltaTime, deltaTime);
	Get(double, TimeScale, timeScale);
private:
	void Update();

	LARGE_INTEGER LI_lastTime;
	LARGE_INTEGER LI_nowTime;
	
	double frequency;
	double lastTime;
	double startTime;
};