#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 directionVec;
layout(location = 1) in vec3 unitNormal;
layout(location = 2) in vec3 unitToLight;
layout(location = 3) in float lightIntensity;


layout(set = 0, binding = 0) uniform samplerCube texSampler;

layout(location = 0) out vec4 outColor;

void main() 
{
	float lightCoef = max(dot(unitNormal, unitToLight), 0.2f) * lightIntensity;
	//outColor = vec4(lightCoef,lightCoef,lightCoef,1.0) * texture(texSampler, fragTexCoord);
	outColor = texture(texSampler, directionVec);
}