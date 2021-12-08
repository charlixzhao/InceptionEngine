#pragma once

#include <Eigen/Dense>


namespace inceptionengine::dynamics
{

	using Mat = Eigen::Matrix<float, -1, -1>;
	using Vec = Eigen::Matrix<float, -1, 1>;
	using SpatialVel = Eigen::Vector<float, 6>;
	using SpatialForce = Eigen::Vector<float, 6>;
	using SpatialAccel = Eigen::Vector<float, 6>;
	using SpatialInertia = Eigen::Matrix<float, 6, 6>;
	using Mat3x3d = Eigen::Matrix<float, 3, 3>;
	using Mat6x6d = Eigen::Matrix<float, 6, 6>;
	using Vec3d = Eigen::Vector<float, 3>;
	using Vec6d = Eigen::Vector<float, 6>;
	using Vec4d = Eigen::Vector<float, 4>;
	using Mat4x4d = Eigen::Matrix<float, 4, 4>;;
	
	
	Mat3x3d Skew3(Vec3d const& v);

	Mat6x6d Skew6(Vec6d const& v);

	Mat6x6d Skew6Adj(Vec6d const& v);

	SpatialInertia ParallelInertia(Mat3x3d const& Ic, Vec3d const& c, float mass);

	Mat6x6d PlukerTransform(Mat3x3d const& E, Vec3d const& r);
	Mat6x6d PlukerTransformAdj(Mat3x3d const& E, Vec3d const& r);
	Mat6x6d PlukerTransformInv(Mat3x3d const& E, Vec3d const& r);
	Mat6x6d PlukerTransformInvAdj(Mat3x3d const& E, Vec3d const& r);


}
