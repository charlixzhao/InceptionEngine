

#include "IE_PCH.h"

#include "Joint.h"



namespace inceptionengine::dynamics
{
	Joint::Joint(JointType type)
	{
		switch (type)
		{
		case JointType::Fixed: break;
		case JointType::Sperical:
		{
			S.resize(6, 3);
			S << 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;

			q.resize(4);
			q << 1.0, 0.0, 0.0, 0.0;
			qd.resize(3);
			qd << 0.0, 0.0, 0.0;
			qdd.resize(3);
			qdd << 0.0, 0.0, 0.0;
			break;
		}
			
		default: break;
		}
	}

	void Joint::SetType(JointType type)
	{
		Type = type;

		switch (type)
		{
		case JointType::Fixed: break;
		case JointType::Sperical:
		{
			S.resize(6, 3);
			S << 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;

			q.resize(4);
			q << 1.0, 0.0, 0.0, 0.0;
			qd.resize(3);
			qd << 0.0, 0.0, 0.0;
			qdd.resize(3);
			qdd << 0.0, 0.0, 0.0;
			break;
		}

		default: break;
		}
	}

	Mat3x3d Joint::E()
	{
		switch (Type)
		{
		case JointType::Fixed: return Mat3x3d::Identity();
			
		case JointType::Sperical:
			return  2.0f * (Mat3x3d() <<
				(q[0] * q[0] + q[1] * q[1] - 0.5f), (q[1] * q[2] + q[0] * q[3]), (q[1] * q[3] - q[0] * q[2]),
				(q[1] * q[2] - q[0] * q[3]), (q[0] * q[0] + q[2] * q[2] - 0.5f), (q[2] * q[3] + q[0] * q[1]),
				(q[1] * q[3] + q[0] * q[2]), (q[2] * q[3] - q[0] * q[1]), (q[0] * q[0] + q[3] * q[3] - 0.5f)).finished();

		default:
			throw std::runtime_error("");
		}
	}

	Vec3d Joint::r()
	{
		switch (Type)
		{
		case JointType::Sperical:
			return (Vec3d() << 0.0, 0.0, 0.0).finished();
		default:
			throw std::runtime_error("");
		}
		
	}

	void Joint::Step(float dt, float threadshold)
	{

		switch (Type)
		{
		case JointType::Fixed: break; 
		case JointType::Sperical:
		{
			if (std::abs(qdd[0]) < threadshold) qdd[0] = 0.0;
			if (std::abs(qdd[1]) < threadshold) qdd[1] = 0.0;
			if (std::abs(qdd[2]) < threadshold) qdd[2] = 0.0;
			//std::cout << qdd << std::endl;
			Mat P;
			P.resize(4, 3);
			P << -q[1], -q[2], -q[3],
				q[0], -q[3], q[2],
				q[3], q[0], -q[1],
				-q[2], q[1], q[0];

			qd += qdd * dt;
			Vec4d quatdot = 0.5 * P * qd;
			q += quatdot * dt;
			q.normalize();
			
			break;
		}

		default: throw std::runtime_error("");

		}
	}

}
