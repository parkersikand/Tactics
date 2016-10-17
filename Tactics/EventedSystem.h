#pragma once

#ifndef __TACTICS_EVENTED_SYSTEM_H__
#define __TACTICS_EVENTED_SYSTEM_H__


#include "System.h"
#include "Event.h"

namespace Tactics {

	template <typename SystemEventType>
	class EventedSystem : public ECS::EventHandler<SystemEventType>, public virtual ECS::System {
	public:

		~EventedSystem() {
			ECS::EventDispatcher::removeTypedEventHandler<SystemEventType>(*this);
		}

		EventedSystem() {
			ECS::EventDispatcher::registerEventHandler<SystemEventType>(*this);
		}

		virtual void handle(const SystemEventType &) {
			if (getWorld())
				run(*getWorld());
		}

		// dont think we need this, system already specifies this
		//virtual void run(ECS::World &) = 0;

	private:
	};


} // namespace tactics;




#endif
