#version 330 core

in vec2 fragTexcoord;
in vec4 fragNormal;
in vec4 fragTangent;

// Ouput data
out vec4 fragColor;

uniform sampler2D textureSampler;

void main()
{
	fragColor = texture( textureSampler, fragTexcoord );
}