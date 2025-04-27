#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec4 tangent;

out vec2 fragTexcoord;
out vec4 fragNormal;
out vec4 fragTangent;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * position;
	fragTexcoord = texcoord;
	fragNormal = normal;
	fragTangent = tangent;
}

