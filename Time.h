#pragma once
class Time : public Singleton<Time>
{
	friend int main();
	friend class SceneGraph;
public:
	Time();
	~Time();

public:
	//���α׷� ���۽ð����κ��� �帥 �ð�(��)
	double GetNowTimeSinceStart();
	GetMacro(double, DeltaTime, deltaTime);
	GetMacro(double, TimeScale, timeScale);
private:
	void Update();

	LARGE_INTEGER LI_lastTime;
	LARGE_INTEGER LI_nowTime;
	
	double frequency;
	double lastTime;
	double startTime;
};