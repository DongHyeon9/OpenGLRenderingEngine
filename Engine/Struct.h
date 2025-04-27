#pragma once
#include "Enum.h"

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

struct Vertex
{
    Vector4 position{};
    Vector2 texcoord{};
    Vector4 normal{};
    Vector4 tangent{};

    static GLuint GetPositionSize() { return sizeof(position); }
    static GLuint GetTexcoordSize() { return sizeof(texcoord); }
    static GLuint GetNormalSize() { return sizeof(normal); }
    static GLuint GetTangentSize() { return sizeof(tangent); }
};

struct MeshData
{
    std::vector<Vertex> vertices{};
    std::vector<uint32> indices{};
};