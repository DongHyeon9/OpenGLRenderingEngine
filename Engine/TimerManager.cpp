#include "TimerManager.h"

bool TimerManager::Init()
{
    lastTime = std::chrono::system_clock::now();

    return true;
}

float TimerManager::Update()
{
    currentTime = std::chrono::system_clock::now();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    return deltaTime.count();
}
