#pragma once

#include "RunTime/RHI/Renderer/Vertex.h"

namespace inceptionengine
{
	void ComputeNormals(std::vector<Vertex>& vertices, std::vector<uint32_t> const& indices);

}