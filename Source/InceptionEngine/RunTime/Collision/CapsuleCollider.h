#pragma once

namespace inceptionengine
{
	struct CapsuleCollider
	{

		static bool DetectCollision(Vec3f const& bottom1, Vec3f const& top1,
									Vec3f const& bottom2, Vec3f const& top2,
									float r1, float r2);
		~CapsuleCollider();

		float radius = 0.0f;
		//In the coordinate of the TransformComponent's transformation
		Vec3f bottom = {};
		Vec3f top = {};
		bool blocking = true;
		std::function<void()> onCollison = []() {};

	};
}