#version 450
#define MAX_NUM_DIR_LIGHTS 100
#define MAX_NUM_SPOT_LIGHTS 100
#define MAX_NUM_POINT_LIGHTS 100
#define ENABLE_SHADOWS true
#define ATTEN_FACTOR 10.0f
#define SPOT_COS_ANGLE 0.7071f //45 degrees
#define FAR_PLANE 1000.0f


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

struct LightVPMatrix {
	mat4 VP;
};
layout(set = 1, binding = 2) uniform DirLightVPMatricesUBO {
	LightVPMatrix DirLightVPMatrices[MAX_NUM_DIR_LIGHTS];
};

layout(set = 1, binding = 3) uniform sampler2D DirShadowmaps[MAX_NUM_DIR_LIGHTS];

layout(set = 1, binding = 4) uniform SpotLightsUBO {
	Light SpotLights[MAX_NUM_SPOT_LIGHTS];
};

layout(set = 1, binding = 5) uniform SpotLightVPMatricesUBO {
	LightVPMatrix SpotLightVPMatrices[MAX_NUM_SPOT_LIGHTS];
};

layout(set = 1, binding = 6) uniform sampler2D SpotShadowmaps[MAX_NUM_SPOT_LIGHTS];

layout(set = 1, binding = 7) uniform PointLightsUBO {
	Light PointLights[MAX_NUM_POINT_LIGHTS];
};

layout(set = 1, binding = 8) uniform samplerCube PointShadowmaps[MAX_NUM_POINT_LIGHTS];



layout(location = 0) in vec3 wPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec4 outCol;



float EnergyCalculationDir(uint dirLightIndex) {
	float lightIntensity = DirLights[dirLightIndex].intensity;
	if (!ENABLE_SHADOWS) {
		return lightIntensity;
	}


	vec4 light_cPos = DirLightVPMatrices[dirLightIndex].VP * vec4(wPos, 1.0f);
	vec3 projCoords = light_cPos.xyz / light_cPos.w;
	projCoords.xy = projCoords.xy * 0.5f + 0.5f;

	float currentDepth = projCoords.z;
	float closestDepth = texture(DirShadowmaps[dirLightIndex], projCoords.xy).r;
	
	if (currentDepth - 0.00001f <= closestDepth) {
		return lightIntensity;
	}

	return 0.0f;
}

float EnergyCalculationSpot(uint spotLightIndex) {
	vec3 P = wPos - SpotLights[spotLightIndex].pos;
	float dist = length(P);
	P = normalize(P);

	vec3 lightDir = SpotLights[spotLightIndex].dir;
	float lightIntensity = SpotLights[spotLightIndex].intensity;
	if (!ENABLE_SHADOWS) {
		if (dot(P, lightDir) > SPOT_COS_ANGLE) {
			return lightIntensity * clamp(ATTEN_FACTOR / (ATTEN_FACTOR + dist*dist), 0.0f, 1.0f);
		}

		return 0.0f;
	}


	vec4 light_cPos = SpotLightVPMatrices[spotLightIndex].VP * vec4(wPos, 1.0f);
	vec3 projCoords = light_cPos.xyz / light_cPos.w;
	projCoords.xy = projCoords.xy * 0.5f + 0.5f;

	float currentDepth = projCoords.z;
	float closestDepth = texture(SpotShadowmaps[spotLightIndex], projCoords.xy).r;
	
	if (currentDepth - 0.00001f <= closestDepth) {
		if (dot(P, lightDir) > SPOT_COS_ANGLE) {
			return lightIntensity * clamp(ATTEN_FACTOR / (ATTEN_FACTOR + dist*dist), 0.0f, 1.0f);
		}
	}

	return 0.0f;
}

float EnergyCalculationPoint(uint pointLightIndex) {
	vec3 lightPos = PointLights[pointLightIndex].pos;
	float dist = length(wPos - lightPos);

	float lightIntensity = PointLights[pointLightIndex].intensity;
	if (!ENABLE_SHADOWS) {
		return lightIntensity * clamp(ATTEN_FACTOR / (ATTEN_FACTOR + dist*dist), 0.0f, 1.0f);
	}


	vec3 wPosFlip = vec3(-wPos.x, wPos.y, wPos.z);
	vec3 lightPosFlip = vec3(-lightPos.x, lightPos.y, lightPos.z);
	vec3 fragToLight = wPosFlip - lightPosFlip;

	float currentDepth = length(fragToLight);
	float closestDepth = texture(PointShadowmaps[pointLightIndex], fragToLight).r * FAR_PLANE;
	
	if (currentDepth - 0.01f <= closestDepth) {
		return lightIntensity * clamp(ATTEN_FACTOR / (ATTEN_FACTOR + dist*dist), 0.0f, 1.0f);
	}

	return 0.0f;
}

void main(){
	// FLOOR COLORING
	vec4 gridCol;
	
	float thickness = 0.025f;
	float halfThickness = thickness/2.0f;
	float size = 70.0f;

	vec2 uv_ = uv * size;
	//if at boundary double the thickness
	if(uv_.x < thickness || uv_.x > size-thickness || uv_.y < thickness || uv_.y > size-thickness){
		halfThickness = thickness;
	}
	uv_ = fract(uv_);
	
	
	vec4 mainCol = vec4(1.0f, 0.3f, 0.4f, 1.0f);
	mainCol.xyz *= 0.8f;
	vec4 lineCol = vec4(1.0f);
	//set output color to main color by default
	gridCol = mainCol;
	
	//if close to edge, make output color the line color
	if(uv_.x < halfThickness || uv_.x > 1 - halfThickness || uv_.y < halfThickness || uv_.y > 1 - halfThickness){
		gridCol = lineCol;
	}
	


	
	// LIGHTING
	outCol = vec4(0.0f);
	

	vec3 N = normalize(normal);

	for(uint dirLightIndex = 0; dirLightIndex < numLightsUBO.numDirLights; dirLightIndex++){
		Light light = DirLights[dirLightIndex];
		vec3 L = -light.dir;

		// Attenuation
		float attenuation = EnergyCalculationDir(dirLightIndex);

		// Diffuse
		float diffuse = clamp(dot(N, L), 0.0f, 1.0f);
		outCol += gridCol * diffuse * light.col * attenuation;
	}

	for(uint spotLightIndex = 0; spotLightIndex < numLightsUBO.numSpotLights; spotLightIndex++){
		Light light = SpotLights[spotLightIndex];
		vec3 L = normalize(light.pos - wPos);

		// Attenuation
		float attenuation = EnergyCalculationSpot(spotLightIndex);

		// Diffuse
		float diffuse = clamp(dot(N, L), 0.0f, 1.0f);
		outCol += gridCol * diffuse * light.col * attenuation;
	}

	for(uint pointLightIndex = 0; pointLightIndex < numLightsUBO.numPointLights; pointLightIndex++){
		Light light = PointLights[pointLightIndex];
		vec3 L = normalize(light.pos - wPos);

		// Attenuation
		float attenuation = EnergyCalculationPoint(pointLightIndex);
		
		// Diffuse
		float diffuse = clamp(dot(N, L), 0.0f, 1.0f);
		outCol += gridCol * diffuse * light.col * attenuation;
	}
	

	// Ambient
	outCol += gridCol * vec4(vec3(1.0f, 1.0f, 1.0f) * 0.1f, 1.0f);

	outCol = clamp(outCol, vec4(0.0f), vec4(1.0f));
}