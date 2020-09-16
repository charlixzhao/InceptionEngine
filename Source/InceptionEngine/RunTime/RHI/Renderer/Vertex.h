#pragma once

namespace inceptionengine
{
	struct Vertex
	{
		Vec4f position = Vec4f(0.0f);
		Vec4f color = Vec4f(0.0f);
		Vec3f texCoord = Vec3f(0.0f);
		Vec3f vertexNormal = { 0.0f,1.0f,0.0f };
		Vec4f boneWeights = { 1.0,0.0,0.0,0.0 };
		Vec4ui affectedBonesID = { 0,0,0,0 };
	};
}