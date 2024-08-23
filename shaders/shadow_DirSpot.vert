#version 450

layout(push_constant, std430) uniform matrices {
	mat4 light_WVP;
};

layout(location = 0) in vec3 inPos;

void main(){
	gl_Position = light_WVP * vec4(inPos, 1.0f);
}