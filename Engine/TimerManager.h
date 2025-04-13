#pragma once
#include "CoreMinimal.h"

class TimerManager
{
	DECLARE_SINGLETON(TimerManager);
public:

protected:

private:
	std::chrono::system_clock::time_point lastTime{};
	std::chrono::system_clock::time_point currentTime{};
	std::chrono::duration<float> deltaTime{};

public:
	bool Init();
	float Update();
	float GetDeltaTime()const { return deltaTime.count(); }

protected:

private:

};

