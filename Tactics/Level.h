#pragma once


#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "Entity.h"

#include "object3d.h"

#include <string>
#include <vector>

namespace Tactics {

	enum GroundType { Grass, Lava, Stone, Water, Wood, Street};

	class Level {
	public:
		
		static const unsigned int MAX_SIZE = 256;

		// load level from file
		Level(std::string);

		// Create level object from data
		Level(int, int, int*, GroundType*);

		int getHeight() const { return height; }
		int getWidth() const { return width; }

        // Load level data onto GPU
		virtual void load(ECS::World &);

	private:

		// The width and height
		int width, height;

		// Height at each position * 2 (eg, 3 = H1.5)
		int heightMap[MAX_SIZE][MAX_SIZE] = { 0 };

		// Ground type at each point
		GroundType groundMap[MAX_SIZE][MAX_SIZE] = { Grass };

		// Ground decorations
		std::vector<ECS::EntityHdl> decorations;

		// The level is a container of drawables
		std::vector<ECS::EntityHdl> drawables;

	};

	namespace Components {
		// IDEA maybe not use this. Level can be a separate game concept, but it contains entities, fires events, etc...
		// IDEA use an empty component so it can still be managed by the world
		struct Level : public ECS::Component {
			Tactics::Level level;
		};
	}

}


#endif


