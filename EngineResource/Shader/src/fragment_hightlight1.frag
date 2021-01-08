#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 unitNormal;
layout(location = 3) in vec3 unitToLight;
layout(location = 4) in float lightIntensity;

layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) out vec4 outColor;

void main() 
{
	float lightCoef = 1.2;
	outColor = vec4(lightCoef,lightCoef,lightCoef,1.0) * texture(texSampler, fragTexCoord);
}

