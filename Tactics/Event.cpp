

#include "Event.h"


Tactics::ECS::EventDispatcher * Tactics::ECS::EventDispatcher::instance = nullptr;
unsigned int Tactics::ECS::EventDispatcher::eventIndex = 1;


void Tactics::ECS::EventDispatcher::removeEventHandler(Tactics::ECS::BaseEventHandler & hdl) {
	for (unsigned int i = 0; i < ECS_MAX_EVENT_TYPES; i++) {
		auto find = std::find(eventHandlers[i].begin(), eventHandlers[i].end(), &hdl);
		if (find != eventHandlers[i].end()) {
			eventHandlers[i].erase(find);
		}
	}
}
