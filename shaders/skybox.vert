#version 450

layout(push_constant, std430) uniform matrices {
	mat4 WVP;
	mat4 W;
	mat4 normal_W;
	uint textureIndex;
};



layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inCol;

layout(location = 0) out vec3 wPos;


void main(){
	gl_Position = WVP * vec4(inPos, 1.0f);

	wPos = (W * vec4(inPos, 1.0f)).xyz;
}