#pragma once

//#include "Math/Spatial.h"

namespace inceptionengine::dynamics
{
	enum class JointType
	{
		None,
		Fixed,
		RevoluteZ,
		Sperical
	};

	struct Joint
	{
		Joint() { ; }

		Joint(JointType type);

		void SetType(JointType type);

		//transformation that transforms coordinate vectors, rathern than transform frame
		Mat3x3d E();

		//joint relative rotation 
		Mat3x3d R() { return E().transpose(); }

		//joint relative translation
		Vec3d r();

		void Step(float dt, float threadshold);

		JointType Type = JointType::None;

		Mat S;  //motion subspace
		Vec q;
		Vec qd;
		Vec qdd;


	};
}

