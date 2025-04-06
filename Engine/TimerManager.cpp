#include "TimerManager.h"

bool TimerManager::Init()
{
    lastTime = glfwGetTime();

    return true;
}

float TimerManager::Update()
{
    currentTime = glfwGetTime();
    deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;
    return deltaTime;
}
