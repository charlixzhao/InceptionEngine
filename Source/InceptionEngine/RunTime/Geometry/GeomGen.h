#pragma once

#include "RunTime/SkeletalMesh/SkeletalMesh.h"


namespace inceptionengine
{
	SubMesh GeneratePlane(float scale, float level = 0.0f, std::string const& texture = "StandAloneResource/T_Ground.jpg");
	SubMesh GenerateCuboid(float x, float y, float z, std::string const& texture);
	SubMesh GenerateSphere(float r, std::string const& texture);
}



