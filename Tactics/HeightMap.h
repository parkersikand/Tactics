#pragma once

#ifndef __TACTICS_HEIGHT_MAP_H__
#define __TACTICS_HEIGHT_MAP_H__

#include "Component.h"
#include "Object3D.h"

namespace Tactics {

	namespace HeightMap {

		class HeightMap {
		public:
			HeightMap(unsigned int w, unsigned int h);

			static HeightMap MakeHeightMap(Tactics::Components::CObject3D *, glm::vec2 offset = glm::vec2(0.f));

			void writeFile(const char * filename);

			float getWidth() { return width; }
			float getHeight() { return height; }

			float get(unsigned int x, unsigned int y);
			float getInterpolated(float x, float y);

		private:
			unsigned int width, height;
			glm::vec3 offset;
			std::vector<float> heightMap;

			struct HeightMapProgramManager {
				HeightMapProgramManager();
				static HeightMapProgramManager * Instance;
				static HeightMapProgramManager * getInstance();
				GLuint programId;
			};

		};

		namespace Components {

			struct HasHeightMap : public ECS::Component {
				HeightMap heightMap;
			};

		}

	}

}

#endif
