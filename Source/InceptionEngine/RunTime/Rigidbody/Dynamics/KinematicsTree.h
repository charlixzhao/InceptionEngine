#pragma once

//#include "Math/Spatial.h"
#include "Joint.h"
#include "Rigidbody.h"


namespace inceptionengine::dynamics
{
	struct KinematicsTree
	{

		KinematicsTree();

		KinematicsTree(int n, int m);

		~KinematicsTree();

		Vec f(Vec const& x, std::vector<Vec3d> const& jointTorques, std::vector<SpatialForce> const& ext);

		void AddCuboidLink(JointType jointType, float x, float y, float z, float r, int parent, Vec3d const& offset);

		//void ForwardDynamics(std::vector<Vec3d> const& jointTorques, std::vector<SpatialForce> const& ext);

		//void Step(float dt, float threadshold = 0.0);

		int N = 0;
	
		std::vector<Rigidbody> Bodies; //vector of single rigidbodies

		std::vector<int> Parents;  //topology info

		std::vector<Joint> Joints;  //joint i connect body i to that body's parent
		
		std::vector <std::pair<Mat3x3d, Vec3d>> Geoms;  //The body-fixed rotation and position of joint's predecessor frame in its parent's successor frame

		std::vector<SpatialInertia> Io; 

		void InitializeComputationBuffer();

	private:
		//computation funcs

		void SetJointPdot(int i, Vec4d const& p, Vec3d const& qd);

		void SetJointQddot(int i, Vec3d const& qdd);

	private:
		//computation buffer
		std::vector<SpatialVel> bodyVels;
		std::vector<SpatialAccel> bodyAccels;
		Vec xd;
		std::vector<Mat6x6d> IA;
		std::vector<Vec6d> PA;
		std::vector<Mat> D;
		std::vector<Mat6x6d> X;
		Mat P;
	
	};
}

