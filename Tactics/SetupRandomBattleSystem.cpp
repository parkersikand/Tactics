
#include "SetupRandomBattleSystem.h"

#include "Actor.h"

#include "Util.h"

using namespace Tactics;
using namespace ECS;

void SetupRandomBattleSystem::run(World & world) {
	// create between 4-7 enemies
	int enemyCount = 4 + (rand() % 4);

	std::vector<EntityHdl> enemiesCreated;

	for (int i = 0; i < enemyCount; i++) {
		EntityHdl e = world.newEntity();
		auto * stats = world.addComponent<Actor::Components::BaseStats>(e);
		for (auto p : stats->stats) {
			p.second = rand() % 10;
		}
		// TODO add base stats based on level
	}

	// create a random height map
	int w = 20 + (rand() % 11);
	int h = 20 + (rand() % 11);

	int ** hw = new int*[h];
	for (int i = 0; i < h; i++) hw[i] = new int[w];

	for (int hi = 0; hi < h; hi++) {
		for (int wi = 0; wi < w; wi++) {
			std::vector<double> vals;
			if (hi > 0) vals.push_back(hw[hi - 1][wi]);
			if (wi > 0) vals.push_back(hw[hi][wi - 1]);
			if (vals.size() > 0) {
				hw[hi][wi] = (int)(avg(vals) + (rand() % 5) - 2);
			}
			else {
				hw[hi][wi] = rand() % 4;
			}
		}
	}

	for (int i = 0; i < h; i++) delete hw[i];
	delete hw;

}

