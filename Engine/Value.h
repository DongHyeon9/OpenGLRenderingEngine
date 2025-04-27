#pragma once
#include "Struct.h"

namespace SHADER_EXTENSION
{
	constexpr std::string_view VERTEX{ ".vert" };
	constexpr std::string_view FRAGMENT{ ".frag" };
}

namespace GLOBAL
{
	constexpr float UNIT{ 0.001f };

	constexpr Vector4 FORWARD{ 0.f,0.f,-1.f, 0.0f };
	constexpr Vector4 BACKWARD{ 0.f,0.f,1.f, 0.0f };
	constexpr Vector4 RIGHT{ 1.f,0.f,0.f, 0.0f };
	constexpr Vector4 LEFT{ -1.f,0.f,0.f, 0.0f };
	constexpr Vector4 UP{ 0.f,1.f,0.f, 0.0f };
	constexpr Vector4 DOWN{ 0.f,-1.f,0.f, 0.0f };
}