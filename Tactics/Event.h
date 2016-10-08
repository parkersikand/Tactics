#pragma once

#ifndef __ECS_EVENT_H__
#define __ECS_EVENT_H__

#include "Component.h"
#include "Entity.h"
#include "System.h"
#include "Constants.h"

//#define MAX_EVENT_TYPES 2048

namespace Tactics {

	namespace ECS {

		// empty Event base class
		// event sub-types manually set parent in constructor
		class Event {
		public:
		private:
			Event * parent = NULL;
			friend class EventDispatcher;
		};


		// Event dispatched when an entity gains a new component
		template <typename ComponentType>
		struct ComponentAddedEvent : Event {
			EntityHdl hdl;
		};


		class BaseEventHandler {
		public:
			virtual void handle(const Event &) = 0;
		};


		// Event handler interface
		template <typename EventType>
		class EventHandler : public BaseEventHandler {
		public:
			virtual void handle(const Event &) {};
			virtual void handle(const EventType &) = 0;
		};

		// global event dispatcher
		// holds listeners
		class EventDispatcher {
		public:
			static EventDispatcher * getInstance() {
				if (!instance) instance = new EventDispatcher;
				return instance;
			}

			template <typename EventType>
			static void registerEventHandler(EventHandler<EventType> & eh) {
				// put this handler in its slot
				getInstance()->eventHandlers[getEventId<EventType>()].push_back(&eh);
			}

			// this should probably be private
			template <typename EventType>
			inline static unsigned int getEventId() {
				static unsigned int id = 0;
				if (id > 0) return id;
				id = EventDispatcher::getInstance()->eventIndex;
				EventDispatcher::getInstance()->eventIndex++;
				return id;
			}

			// Post an event to be broadcast to appropriate handlers
			template <typename EventType>
			static void postEvent(EventType e) {
				for (auto const & ehptr : getInstance()->eventHandlers[EventDispatcher::getEventId<EventType>()]) {
					EventHandler<EventType> *ptr = dynamic_cast<EventHandler<EventType> *>(ehptr);
					ptr->handle(e);
				}
				// TODO re broadcast event as parent type
			}

		private:
			static EventDispatcher * instance;
			static unsigned int eventIndex;

			std::vector<BaseEventHandler *> eventHandlers[ECS_MAX_EVENT_TYPES];
		}; // class EventDispatcher


	} // namespace ECS

} // namespace Tactics




#endif

