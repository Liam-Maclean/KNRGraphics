#pragma once

#include "ecs/simulation/ecs_system.h"
#include "data_structures/state_machine.hpp"

#include "animation.h"

namespace Ecs
{
	namespace Events
	{
		struct EventOnAnimationBegin
		{};

		struct EventOnAnimationEnd
		{};

		struct EventStateTransition
		{
			entt::entity EntityId = entt::null;
			HashString GraphId;
			HashString NextState;
		};
	}
}


namespace QAnim
{
	class AnimationStateMachine 
		: public StateMachine<Animation2D>
	{

	};

	class ProcessAnimation
		: public IProcess
	{
		void OnTick(entt::registry& worldData, const GameTime& gameTime) override;
	};

	class AnimationSystem final
		: public EcsSystem
	{
	public:
		AnimationSystem(entt::registry& registry);
		virtual void Tick(const GameTime& gameTime) override;

		void SetGraph(const HashString& id, AnimationStateMachine& graph)
		{
			m_graphs.emplace(std::make_pair(id, std::move(graph)));
		}

		AnimationStateMachine& GetGraph(const HashString& id)
		{
			return m_graphs[id];
		}

	private:
		std::map<HashString, AnimationStateMachine, HashStringCmp> m_graphs;
	};
}