#version 450

layout(location = 0) in vec4 lightCol;

layout(location = 0) out vec4 outCol;


void main(){
	outCol = lightCol;
}