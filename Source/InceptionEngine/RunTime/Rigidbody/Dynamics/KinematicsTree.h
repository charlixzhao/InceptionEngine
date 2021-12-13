#pragma once

//#include "Math/Spatial.h"
#include "Joint.h"
#include "Rigidbody.h"


namespace inceptionengine::dynamics
{
	struct KinematicsTree
	{
		KinematicsTree(int n, int m);

		~KinematicsTree();

		void ForwardDynamics(std::vector<Vec3d> const& jointTorques, std::vector<SpatialForce> const& ext);

		void Step(float dt, float threadshold = 0.0)
		{
			for (auto& j : Joints) j.Step(dt, threadshold);
		}

		int N = 0;
	
		std::vector<Rigidbody> Bodies; //vector of single rigidbodies

		std::vector<int> Parents;  //topology info

		std::vector<Joint> Joints;  //joint i connect body i to that body's parent
		
		std::vector <std::pair<Mat3x3d, Vec3d>> Geoms;  //The body-fixed rotation and position of joint's predecessor frame in its parent's successor frame

		std::vector<SpatialInertia> Io; 
	
	};
}

