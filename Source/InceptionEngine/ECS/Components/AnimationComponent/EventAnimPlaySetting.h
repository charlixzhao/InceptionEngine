#pragma once

#include "AnimNotify.h"
#include "AnimNotifyState.h"
#include "AnimSpeedRange.h"

namespace inceptionengine
{
	struct EventAnimPlaySetting
	{
		std::string animFilePath;
		bool rootMotion = false;
		float blendInDuration = 0.05f;
		float blendOutDuration = 0.05f;
		std::vector<AnimSpeedRange> animSpeedRanges;

		std::function<void()> animStartCallback = []() {; };
		std::function<void()> animInterruptCallback = []() {; };
		std::function<void()> animEndCallback = []() {; };
		std::vector<AnimNotify> animNotifies;
		std::vector<AnimNotifyState> animNotifyStates;
	};
}