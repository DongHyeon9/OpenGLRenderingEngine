#include "InputManager.h"

bool InputManager::Init()
{
    return true;
}

void InputManager::Update()
{
    for (auto& keyState : keyStates)
    {
        switch (keyState.second)
        {
        case E_KEY_STATE::DOWN: keyState.second = E_KEY_STATE::PRESS; break;
        case E_KEY_STATE::UP: keyState.second = E_KEY_STATE::NONE; break;
        }
    }
}

void InputManager::UpdateKeyState(int Key, int Action)
{
    E_KEY_TYPE key = static_cast<E_KEY_TYPE>(Key);
    switch (Action)
    {
    case GLFW_RELEASE: keyStates[key] = E_KEY_STATE::UP; break;
    case GLFW_PRESS: keyStates[key] = E_KEY_STATE::DOWN; break;
    }
}
