
#include "IE_PCH.h"
#include "RigidBody.h"

namespace inceptionengine::dynamics
{
	Rigidbody ConstructCuboid(float x, float y, float z, float density)
	{
		float mass = (x * 100.0f) * (y * 100.0f) * (z * 100.0f) * density / 1000.0f;
		Rigidbody b;
		b.m = mass;
		b.Ic <<
			mass * (y * y + z * z) / 12.0f, 0.0f, 0.0f,
			0.0f, mass* (x * x + z * z) / 12.0f, 0.0f,
			0.0f, 0.0f, mass* (x * x + y * y) / 12.0f;
		return b;
	}
}