#include "RigidBody.h"

namespace inceptionengine::dynamics
{
	Rigidbody ConstructCuboid(float mass, float x, float y, float z)
	{
		Rigidbody b;
		b.m = mass;
		b.Ic <<
			mass * (y * y + z * z) / 12.0f, 0.0f, 0.0f,
			0.0f, mass* (x * x + z * z) / 12.0f, 0.0f,
			0.0f, 0.0f, mass* (x * x + y * y) / 12.0f;
		return b;
	}
}