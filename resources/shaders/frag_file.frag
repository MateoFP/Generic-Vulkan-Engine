#version 450

layout(set = 1, binding = 0) uniform sampler samp;
layout(set = 1, binding = 1) uniform texture2D tex[3];

layout(location = 0) flat in uint inTexIndex;
layout(location = 1) in vec2 inUv;

layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = texture(sampler2D(tex[inTexIndex], samp), inUv); 
	//outColor = vec4(inUv, 0,0);
}