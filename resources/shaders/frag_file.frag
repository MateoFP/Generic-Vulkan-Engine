#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D furnace_tex;

void main() 
{
    outColor = texture(furnace_tex, fragTexCoord); 
}