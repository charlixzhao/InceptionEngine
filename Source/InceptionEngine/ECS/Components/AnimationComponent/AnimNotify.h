#pragma once

namespace inceptionengine
{
	struct AnimNotify
	{
		float ratio = 0.0f;
		std::function<void()> notify = []() {};
		bool notified = false;
	};
}