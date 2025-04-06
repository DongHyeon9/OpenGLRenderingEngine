#pragma once
#include "CoreMinimal.h"

class TimerManager
{
	DECLARE_SINGLETON(TimerManager);
public:

protected:

private:
	double lastTime{};
	double currentTime{};
	float deltaTime{};

public:
	bool Init();
	float Update();
	float GetDeltaTime()const { return deltaTime; }

protected:

private:

};

