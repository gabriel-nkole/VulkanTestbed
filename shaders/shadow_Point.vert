#version 450
#extension GL_EXT_multiview : enable
#define MAX_NUM_POINT_LIGHTS 100

layout(push_constant, std430) uniform matrices {
	mat4 light_W;
	uint pointLightIndex;
};


layout(set = 0, binding = 0) uniform NumLightsUBO {
	uint numDirLights;
	uint numSpotLights;
	uint numPointLights;
} numLightsUBO;

struct VPMatricesArr {
	mat4 VPMatrices[6];
};
layout(set = 0, binding = 1) uniform PointLightVPMatricesUBO {
	VPMatricesArr PointLightVPMatrices[MAX_NUM_POINT_LIGHTS];
};


layout(location = 0) in vec3 inPos;

layout(location = 0) out vec3 wPos;


void main(){
	vec4 wPos4 = light_W * vec4(inPos, 1.0f);
	wPos = wPos4.xyz;
	gl_Position = PointLightVPMatrices[pointLightIndex].VPMatrices[gl_ViewIndex] * wPos4;
}