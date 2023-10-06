#include <qlpch.h>

#include "animation_system.h"

namespace QAnim
{
	void ProcessAnimation::OnTick(entt::registry& registry, const GameTime& gameTime)
	{
		auto view = registry.view<AnimationComponent>();
		for (auto entity : view)
		{
			AnimationComponent animation = view.get<AnimationComponent>(entity);

			if (animation.Frames.size() > 0)
			{
				animation.Counter += gameTime.TimeStep;

				bool bLoop = animation.AnimationState == EAnimationPlayback::Repeat;

				if (animation.FrameIndex == animation.Frames.size() - 1 && !bLoop)
				{
					return;
				}

				if (animation.Counter >= animation.Frames[animation.FrameIndex].TimeInterval)
				{
					animation.Counter = 0.0f;
					animation.Frames[animation.FrameIndex].SpriteSheetIndex;
					animation.FrameIndex = animation.FrameIndex++ % animation.Frames.size();
				}
			}
		}
	}

	AnimationSystem::AnimationSystem(entt::registry& registry)
		: EcsSystem(registry)
	{
		
	}

	void AnimationSystem::Tick(const GameTime& gameTime)
	{
		auto events = g_registry.view<Ecs::Events::EventStateTransition>();
		for (auto entity : events)
		{
			Ecs::Events::EventStateTransition& evt = events.get<Ecs::Events::EventStateTransition>(entity);
			AnimationComponent anim = g_registry.get<AnimationComponent>(evt.EntityId);

			auto& graph = GetGraph(evt.GraphId);
			auto nextAnim = graph.GetState(evt.NextState).GetData();

			anim.Counter = 0.0f;
			anim.FrameIndex = 0;
			anim.Frames = nextAnim.Frames;
			anim.AnimationState = nextAnim.AnimationState;

			g_registry.replace<AnimationComponent>(evt.EntityId, anim);

			g_registry.destroy(entity);
		}

		auto view = g_registry.view<AnimatorComponent, AnimationComponent>();
		for (auto entity : view)
		{
			AnimationComponent animation = view.get<AnimationComponent>(entity);
			AnimatorComponent animator = view.get<AnimatorComponent>(entity);

			bool readyToTransition = false;

			if (animation.Frames.size() > 0)
			{
				animation.Counter += gameTime.TimeStep;
				if (animation.Counter >= animation.Frames[animation.FrameIndex].TimeInterval)
				{
					animation.Counter = 0.0f;
					animation.Frames[animation.FrameIndex].SpriteSheetIndex;
					animation.FrameIndex = ++animation.FrameIndex % animation.Frames.size();
					readyToTransition = true;
				}
			}

			g_registry.replace<AnimationComponent>(entity, animation);

			for (auto& entry : animator.Transitions)
			{
				bool& shouldTransition = entry.second;

				if (shouldTransition == true && readyToTransition)
				{
					animator.NextStateId = entry.first;

					Ecs::Events::EventStateTransition evt =
					{
						entity,
						animator.GraphId,
						animator.NextStateId,
					};
					animator.CurrentStateId = animator.NextStateId;
					shouldTransition = false;
				}
			}

			g_registry.replace<AnimatorComponent>(entity, animator);
		}
	}

}