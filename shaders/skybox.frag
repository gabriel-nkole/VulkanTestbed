#version 450

layout(set = 0, binding = 0) uniform samplerCube Skybox;



layout(location = 0) in vec3 wPos;

layout(location = 0) out vec4 outCol;


void main(){
	outCol = texture(Skybox, wPos);
}