#include "SkyboxComponent.h"


namespace inceptionengine
{

	std::vector<RenderUnit>& SkyboxComponent::GetRenderUnits(unsigned int i)
	{
		return mRenderUnits[i];
	}
	RenderObejctID SkyboxComponent::GetRenderObejctID() const
	{
		return mRenderObejctID;
	}
	std::vector<Vertex> SkyboxComponent::GetVertices()
	{
		std::array<Vec4f, 8> const vertexPositions =
		{
			Vec4f(-1.0f, -1.0f, -1.0f, 1.0f),
			Vec4f(1.0f, -1.0f, -1.0f, 1.0f),
			Vec4f(1.0f, -1.0f, 1.0f, 1.0f),
			Vec4f(-1.0f, -1.0f, 1.0f, 1.0f),
			Vec4f(-1.0f, 1.0f, -1.0f, 1.0f),
			Vec4f(1.0f, 1.0f, -1.0f, 1.0f),
			Vec4f(1.0f, 1.0f, 1.0f, 1.0f),
			Vec4f(-1.0f, 1.0f, 1.0f, 1.0f)
		};

		std::vector<Vertex> vertices;
		vertices.reserve(8);

		for (int i = 0; i < 8; i++)
		{
			Vertex v;
			v.position = vertexPositions[i];
			vertices.push_back(std::move(v));
		}

		return vertices;
	}
	std::vector<unsigned int> SkyboxComponent::GetIndices()
	{
		return { 0,1,5, 5,4,0, 2,3,7,7,6,2, 4,5,6, 6,7,4,  0,3,2, 2,1,0, 1,2,6, 6,5,1, 0,4,7, 7,3,0 };
	}
}

