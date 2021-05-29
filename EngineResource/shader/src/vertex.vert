#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform MVP
{
	mat4 view;
	mat4 proj;
}mvp;

layout(set = 0, binding = 0) uniform TransformationBuffer
{
	mat4 t[200];
}tBuffer;


layout(set = 0, binding = 2) uniform Light
{
	vec4 locationAndIntensity;
}light;


layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 texCoord;
layout(location = 3) in vec3 vertexNormal;
layout(location = 4) in vec4 boneWeights;
layout(location = 5) in uvec4 boneID;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 unitNormal;
layout(location = 3) out vec3 unitToLight;
layout(location = 4) out float lightIntensity;

void main() 
{
	mat4 boneTrans = boneWeights[0] * tBuffer.t[boneID[0]] + boneWeights[1] * tBuffer.t[boneID[1]] + boneWeights[2] * tBuffer.t[boneID[2]] + boneWeights[3] * tBuffer.t[boneID[3]];
	vec4 vertexPosition = tBuffer.t[1] * boneTrans * pos;
	gl_Position = mvp.proj * mvp.view * vertexPosition;
	fragColor = color;
	fragTexCoord = vec2(texCoord.x, texCoord.y);
	unitNormal = normalize((tBuffer.t[1] * boneTrans * vec4(vertexNormal, 0.0)).xyz);	
	unitToLight = normalize( (light.locationAndIntensity).xyz - vertexPosition.xyz );
	lightIntensity = light.locationAndIntensity.w;
}
