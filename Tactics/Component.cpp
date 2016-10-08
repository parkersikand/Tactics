
#include "Component.h"

using namespace Tactics::ECS;

void ComponentMask::init() {
}

bool Tactics::ECS::operator==(const ComponentMask & a, const ComponentMask & b)
{
	return a.mask == b.mask;
}

ComponentManager * ComponentManager::instance = NULL;

ComponentManager::~ComponentManager() {
	for (auto it : prototypes) {
		if (it.second) delete it.second;
	}
}
