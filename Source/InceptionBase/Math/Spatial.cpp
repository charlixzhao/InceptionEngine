#include "Spatial.h"



namespace inceptionengine::dynamics
{
	

	Mat3x3d Skew3(Vec3d const& v)
	{
		return (Mat3x3d() <<
			0.0, -v[2], v[1],
			v[2], 0.0, -v[0],
			-v[1], v[0], 0.0).finished();
	}

	
	Mat6x6d Skew6(Vec6d const& v)
	{
		auto const& w = v.block<3, 1>(0, 0);
		auto const& v0 = v.block<3, 1>(3, 0);
		Mat6x6d m = Mat6x6d::Zero();
		m.block<3, 3>(0, 0) = Skew3(w);
		m.block<3, 3>(3, 0) = Skew3(v0);
		m.block<3, 3>(3, 3) = Skew3(w);
		return m;
	}

	Mat6x6d Skew6Adj(Vec6d const& v)
	{
		auto const& w = v.block<3, 1>(0, 0);
		auto const& v0 = v.block<3, 1>(3, 0);
		Mat6x6d m = Mat6x6d::Zero();
		m.block<3, 3>(0, 0) = Skew3(w);
		m.block<3, 3>(0, 3) = Skew3(v0);
		m.block<3, 3>(3, 3) = Skew3(w);
		return m;
	}


	SpatialInertia ParallelInertia(Mat3x3d const& Ic, Vec3d const& c, float mass)
	{
		Mat3x3d sc = Skew3(c);
		Mat3x3d sct = sc.transpose();

		SpatialInertia Io;
		Io.block<3, 3>(0, 0) = Ic + mass * sc * sct;
		Io.block<3, 3>(0, 3) = mass * sc;
		Io.block<3, 3>(3, 0) = mass * sct;
		Io.block<3, 3>(3, 3) = mass * Mat3x3d::Identity();
		return Io;
	}

	//pp.22 on Rigid Body Dynamics Algorithms
	Mat6x6d PlukerTransform(Mat3x3d const& E, Vec3d const& r)
	{
		Mat6x6d m = Mat6x6d::Zero();
		m.block<3, 3>(0, 0) = E;
		m.block<3, 3>(3, 0) = -E * Skew3(r);
		m.block<3, 3>(3, 3) = E;
		return m;
	}

	Mat6x6d PlukerTransformAdj(Mat3x3d const& E, Vec3d const& r)
	{
		Mat6x6d m = Mat6x6d::Zero();
		m.block<3, 3>(0, 0) = E;
		m.block<3, 3>(0, 3) = -E * Skew3(r);
		m.block<3, 3>(3, 3) = E;
		return m;
	}

	Mat6x6d PlukerTransformInv(Mat3x3d const& E, Vec3d const& r)
	{
		Mat6x6d m = Mat6x6d::Zero();
		m.block<3, 3>(0, 0) = E.transpose();
		m.block<3, 3>(3, 0) = Skew3(r) * E.transpose();
		m.block<3, 3>(3, 3) = E.transpose();
		return m;
	}

	Mat6x6d PlukerTransformInvAdj(Mat3x3d const& E, Vec3d const& r)
	{
		Mat6x6d m = Mat6x6d::Zero();
		m.block<3, 3>(0, 0) = E.transpose();
		m.block<3, 3>(0, 3) = Skew3(r) * E.transpose();
		m.block<3, 3>(3, 3) = E.transpose();
		return m;
	}
	

}

