#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform MVP
{
	mat4 view;
	mat4 proj;
}mvp;


layout(set = 0, binding = 1) uniform Light
{
	vec4 locationAndIntensity;
}light;


layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;
layout(location = 2) in vec3 texCoord;
layout(location = 3) in vec3 vertexNormal;


layout(location = 0) out vec3 directionVec;
layout(location = 1) out vec3 unitNormal;
layout(location = 2) out vec3 unitToLight;
layout(location = 3) out float lightIntensity;

void main() 
{
	mat4 camera = mvp.view;
	camera[3] = vec4(0,0,0,1);
	gl_Position = mvp.proj * camera * pos;
	directionVec = normalize(pos.xyz);
	unitNormal = vertexNormal;	
	unitToLight = normalize( (light.locationAndIntensity).xyz - pos.xyz );
	lightIntensity = light.locationAndIntensity.w;
}

