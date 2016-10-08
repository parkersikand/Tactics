#ifdef TESTING

#include <gtest/gtest.h>

#include "Event.h"

#include "EventedSystem.h"

using namespace Tactics;
using namespace Tactics::ECS;

struct SimpleEvent : public ECS::Event {
	std::string message = "";
};

struct SimpleEventHandler : public EventHandler<SimpleEvent> {
	virtual void handle(const SimpleEvent & se) {
		worked = se.message == "message";
	}
	bool worked = false;
};


TEST(Events, simple_event_handling) {
	SimpleEventHandler seh;
	EventDispatcher::registerEventHandler(seh);
	SimpleEvent se;
	se.message = "message";
	EventDispatcher::postEvent(se);
	ASSERT_TRUE(seh.worked) << "The handler should get the event and update the variable";
}

struct DummyEvent : public Event {};

struct DummyEventedSystem : public EventedSystem<DummyEvent> {
	virtual void run(World &) {
		success = true;
	}
	bool success = false;
};

TEST(EventedSystem, dummy_evented_system) {
	DummyEventedSystem des;
	EventDispatcher::postEvent(DummyEvent());
	ASSERT_TRUE(des.success) << "The dummy system should fire upon receiving the event";
}



#endif
