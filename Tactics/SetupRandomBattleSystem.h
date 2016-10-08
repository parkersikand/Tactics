#pragma once

#ifndef __TACTICS_RANDOM_BATTLE_SYSTEM_H__
#define __TACTICS_RANDOM_BATTLE_SYSTEM_H__

#include "System.h"
#include "EventedSystem.h"

namespace Tactics {

	// Event gets fired to signal start of random battle
	struct StartRandomBattle : public ECS::Event {};

	// When this system runs, it sets up a battle with random opponents
	class SetupRandomBattleSystem : public EventedSystem<StartRandomBattle> {
	public:
		void run(ECS::World & world);
	private:
	};

}



#endif
