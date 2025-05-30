#pragma once
#include "Type.h"

enum class E_KEY_TYPE : uint16
{
	UP = GLFW_KEY_UP,
	DOWN = GLFW_KEY_DOWN,
	LEFT = GLFW_KEY_LEFT,
	RIGHT = GLFW_KEY_RIGHT,

	KEY_1 = GLFW_KEY_1,
	KEY_2 = GLFW_KEY_2,
	KEY_3 = GLFW_KEY_3,
	KEY_4 = GLFW_KEY_4,
	KEY_5 = GLFW_KEY_5,
	KEY_6 = GLFW_KEY_6,
	KEY_7 = GLFW_KEY_7,
	KEY_8 = GLFW_KEY_8,
	KEY_9 = GLFW_KEY_9,
	KEY_0 = GLFW_KEY_0,

	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_M,
	N = GLFW_KEY_N,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z,

	F1 = GLFW_KEY_F1,
	F2 = GLFW_KEY_F2,
	F3 = GLFW_KEY_F3,
	F4 = GLFW_KEY_F4,
	F5 = GLFW_KEY_F5,
	F6 = GLFW_KEY_F6,
	F7 = GLFW_KEY_F7,
	F8 = GLFW_KEY_F8,
	F9 = GLFW_KEY_F9,
	F10 = GLFW_KEY_F10,
	F11 = GLFW_KEY_F11,
	F12 = GLFW_KEY_F12,

	SPACE = GLFW_KEY_SPACE,
	ESCAPE = GLFW_KEY_ESCAPE,
	ENTER = GLFW_KEY_ENTER,

	L_BUTTON = GLFW_MOUSE_BUTTON_LEFT,
	M_BUTTON = GLFW_MOUSE_BUTTON_MIDDLE,
	R_BUTTON = GLFW_MOUSE_BUTTON_RIGHT,

	L_SHIFT = GLFW_KEY_LEFT_SHIFT,
	L_CTRL = GLFW_KEY_LEFT_CONTROL,
	L_ALT = GLFW_KEY_LEFT_ALT,

	R_SHIFT = GLFW_KEY_RIGHT_SHIFT,
	R_CTRL = GLFW_KEY_RIGHT_CONTROL,
	R_ALT = GLFW_KEY_RIGHT_ALT,
};

enum class E_KEY_STATE : uint8
{
	NONE = 0,
	PRESS,
	DOWN,
	UP,

	END,
};