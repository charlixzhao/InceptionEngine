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
		Joint() = default;

		Joint(JointType type);

		void SetType(JointType type);

		static Mat3x3d E(Vec4d const& p, JointType type);

		//transformation that transforms coordinate vectors, rathern than transform frame
		//Mat3x3d E();

		//joint relative rotation 
		//Mat3x3d R() { return E().transpose(); }

		//joint relative translation
		Vec3d r();

		void Step(float dt, float threadshold);

		JointType Type = JointType::None;

		Mat S;  //motion subspace
		Mat ST;
		//Vec q;
		//Vec qd;
		//Vec qdd;


	};
}

