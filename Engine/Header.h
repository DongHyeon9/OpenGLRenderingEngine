#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

#include <type_traits>

#include <algorithm>
#include <cmath>
#include <format>
#include <memory>
#include <functional>
#include <chrono>

#include <vector>
#include <map>
#include <set>
#include <unordered_map>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#pragma warning(disable:4711 4710 4100 4514 4626 4774 4365 4625 4464 4571 4201 5026 5027 5039)

#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtx\string_cast.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#ifdef _DEBUG
#pragma comment(lib, "OpenGL32.Lib")
#pragma comment(lib, "glew32d.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "GlU32.Lib")
#else
#pragma comment(lib, "OpenGL32.Lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "GlU32.Lib")
#endif