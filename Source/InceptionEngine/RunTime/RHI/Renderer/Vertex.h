#pragma once

namespace inceptionengine
{
	struct Vertex
	{
		template<typename Archive>
		void serialize(Archive& archive)
		{
			archive(position, color, texCoord, normal, boneWeights, affectedBonesID);
		}

		Vec4f position = Vec4f(0.0f);
		Vec4f color = Vec4f(0.0f);
		Vec3f texCoord = Vec3f(0.0f);
		Vec3f normal = { 0.0f,1.0f,0.0f };
		Vec4f boneWeights = { 1.0,0.0,0.0,0.0 };
		Vec4ui affectedBonesID = { 0,0,0,0 };
	};
}