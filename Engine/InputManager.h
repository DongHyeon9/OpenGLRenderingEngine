#pragma once
#include "CoreMinimal.h"

class InputManager
{
	DECLARE_SINGLETON(InputManager);
public:

protected:

private:
	std::unordered_map<E_KEY_TYPE, E_KEY_STATE> keyStates{};

public:
	bool Init();
	void Update();
	void UpdateKeyState(int Key, int Action);
	bool GetButtonPress(E_KEY_TYPE Key) { return GetState(Key) == E_KEY_STATE::PRESS; }
	bool GetButtonDown(E_KEY_TYPE Key) { return GetState(Key) == E_KEY_STATE::DOWN; }
	bool GetButtonUp(E_KEY_TYPE Key) { return GetState(Key) == E_KEY_STATE::UP; }

protected:

private:
	E_KEY_STATE GetState(E_KEY_TYPE Key) { return keyStates[Key]; }
};

