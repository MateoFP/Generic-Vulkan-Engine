#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform PER_OBJECT
{
	int imgIdx;
}pc;

layout(set = 1, binding = 0) uniform sampler samp;
layout(set = 1, binding = 1) uniform texture2D tex[2];

void main() 
{
    outColor = texture(sampler2D(tex[pc.imgIdx], samp), fragTexCoord); 
}