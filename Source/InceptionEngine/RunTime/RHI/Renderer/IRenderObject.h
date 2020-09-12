#pragma once

#include "Renderer.h"
#include <vector>

namespace inceptionengine
{
	class IRenderObject
	{
	public:
		IRenderObject() = default;

		virtual ~IRenderObject() = default;

		virtual RenderObejctID GetRenderObejctID() const = 0;

		RenderObejctID mRenderObejctID = Renderer::InvalidRenderObjectID;

		virtual std::vector<RenderUnit>& GetRenderUnits(unsigned int i) = 0;

	};
}