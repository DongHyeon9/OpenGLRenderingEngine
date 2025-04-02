#pragma once
#include "Header.h"

union Color {
    struct {
        GLclampf r;
        GLclampf g;
        GLclampf b;
        GLclampf a;
    };
    GLclampf rgba[4];
};

struct WindowDesc
{
    int width{ 1280 };
    int height{ 720 };
    std::string windowTitle{ "OpenGL Rendering Engine" };
    Color clearColor{};
};