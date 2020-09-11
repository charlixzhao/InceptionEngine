
#pragma once
/*
This is the interface of any component that can be draw on the screen, some examples are mesh component, terrain component, or 
skybox component, if it exists.
*/
#include "RunTime/RHI/Renderer/Renderer.h"

namespace inceptionengine
{
	class IRenderComponent
	{

	protected:
		RenderObejctID mRenderObejctID = Renderer::InvalidRenderObjectID;

	private:
		friend class Renderer;

		virtual std::vector<RenderUnit>& GetRenderUnits(unsigned int i) = 0;

	};
}



