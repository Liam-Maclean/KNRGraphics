#pragma once

#include "core/core.h"
#include "data_structures/state_machine.hpp"

namespace QAnim
{
	struct KeyFrame2D
	{
		uint32_t SpriteSheetIndex = 0;
		float	 TimeInterval = 0;
	};

	enum EAnimationPlayback : uint32_t
	{
		Repeat,
		OneShot,
	};

	struct Animation2D
	{
		EAnimationPlayback AnimationState = EAnimationPlayback::Repeat;
		std::vector<KeyFrame2D> Frames;
	};

	struct AnimationComponent
	{
		float Counter = 0.0f;
		uint32_t FrameIndex = 0;
		std::vector<KeyFrame2D> Frames;
		EAnimationPlayback AnimationState = EAnimationPlayback::Repeat;
	};

	struct AnimatorComponent
	{
		// The currently used graph
		HashString GraphId;

		// The active state 
		HashString CurrentStateId;

		HashString NextStateId;

		void SetTransition(const std::string& target, bool state)
		{
			// Get the compount name of current and target
			//HashString transition = HashString(CurrentStateId, target);
			//
			//// if it exists then mark for transition
			//if (Transitions.find(transition) != std::end(Transitions))
			//{
			//	Transitions.at(transition) = state;
			//	NextStateId = HashString(target);
			//}
		}

		std::map<HashString, bool> Transitions;
	};
}


