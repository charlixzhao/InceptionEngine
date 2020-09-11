#pragma once


#include <vector>

namespace inceptionengine
{
	struct RenderUnit;

	class IRenderObject
	{
	public:
		//RenderObejctID mRenderObejctID = Renderer::InvalidRenderObjectID;

		virtual std::vector<RenderUnit>& GetRenderUnits(unsigned int i) = 0;

	};
}