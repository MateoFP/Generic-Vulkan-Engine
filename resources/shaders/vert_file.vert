#version 450

layout(set = 0, binding = 0) uniform Global_UBO 
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 projView;
} gUBO;

layout(set = 0, binding = 1) uniform Model_UBO
{
    mat4 model[3];
} mUBO;

layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec3 Normal;
layout(location = 1) in vec2 inUv;
layout(location = 2) in uint inTexIndex;
layout(location = 3) in uint inModelIndex;

layout(location = 0) out uint outTexIndex;
layout(location = 1) out vec2 outUv;

void main() 
{
    gl_Position = gUBO.projView * mUBO.model[inModelIndex] * vec4(inPosition, 1);
    outTexIndex = inTexIndex;
    outUv =  inUv;
}