#version 450
#extension GL_EXT_multiview : enable
#define MAX_NUM_POINT_LIGHTS 100
#define FAR_PLANE 1000.0f

layout(push_constant, std430) uniform matrices {
	mat4 light_W;
	uint pointLightIndex;
};


struct Light {
	vec3 pos;
	vec3 dir;
	vec2 rot;
	vec4 col;
	float intensity;
};
layout(set = 0, binding = 2) uniform PointLightsUBO {
	Light PointLights[MAX_NUM_POINT_LIGHTS];
};

layout(location = 0) in vec3 wPos;

void main(){
	float lightDist = length(wPos - PointLights[pointLightIndex].pos);
	lightDist /= FAR_PLANE;

	gl_FragDepth = lightDist;
}