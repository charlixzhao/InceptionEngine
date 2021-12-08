#pragma once
//#include "Math/Spatial.h"

namespace inceptionengine::dynamics
{

	struct Rigidbody
	{
		float m = 0.0f;
		Mat3x3d Ic = Mat3x3d::Identity();
		SpatialVel V = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		SpatialAccel a = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	};

	
	Rigidbody ConstructCuboid(float mass /*kg*/, float x /*meter*/, float y, float z);
}
