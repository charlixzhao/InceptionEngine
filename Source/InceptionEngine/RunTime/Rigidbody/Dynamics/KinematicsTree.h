#pragma once

//#include "Math/Spatial.h"
#include "Joint.h"
#include "Rigidbody.h"


namespace inceptionengine::dynamics
{
	struct KinematicsTree
	{
		KinematicsTree(int n, int m)
		{
			N = n;
			Geoms.resize(n);
			Bodies.resize(n);
			Io.resize(n, SpatialInertia::Identity());
			Parents.resize(n, -1);

			Joints.resize(m);
			Bodies[0].a = (SpatialAccel() << 0.0f, 0.0f, 0.0f, 0.0, 9.81f, 0.0f).finished();
		}

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

