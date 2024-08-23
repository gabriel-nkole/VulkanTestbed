#version 450

#define MAX_NUM_DIR_LIGHTS 100
#define MAX_NUM_SPOT_LIGHTS 100
#define MAX_NUM_POINT_LIGHTS 100

layout(push_constant, std430) uniform matrices {
	mat4 WVP;
	uint lightType;
	uint lightIndex;
};



layout(set = 1, binding = 0) uniform NumLightsUBO {
	uint numDirLights;
	uint numSpotLights;
	uint numPointLights;
} numLightsUBO;

struct Light {
	vec3 pos;
	vec3 dir;
	vec2 rot;
	vec4 col;
	float intensity;
};
layout(set = 1, binding = 1) uniform DirLightsUBO {
	Light DirLights[MAX_NUM_DIR_LIGHTS];
};

layout(set = 1, binding = 4) uniform SpotLightsUBO {
	Light SpotLights[MAX_NUM_SPOT_LIGHTS];
};

layout(set = 1, binding = 7) uniform PointLightsUBO {
	Light PointLights[MAX_NUM_POINT_LIGHTS];
};


layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec4 inCol;

layout(location = 0) out vec4 lightCol;


void main(){
	gl_Position = WVP * vec4(inPos, 1.0f);


	uint numDirLights = numLightsUBO.numDirLights;
	uint numSpotLights = numLightsUBO.numSpotLights;
	uint numPointLights = numLightsUBO.numPointLights;

	lightCol = vec4(0.0f);
	if (lightType == 0) {
		lightCol = DirLights[lightIndex].col;
	}
	
	else if (lightType == 1) {
		lightCol = SpotLights[lightIndex].col;
	}
	
	else if (lightType == 2) {
		lightCol = PointLights[lightIndex].col;
	}

	lightCol = vec4(lightCol.xyz * 2.0f, lightCol.w);
}