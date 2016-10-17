#pragma once

#ifndef __TACTICS_COLORED_DRAW_H__
#define __TACTICS_COLORED_DRAW_H__

#include <GL/glew.h>

#include "DrawSystem.h"


namespace Tactics {

	namespace Systems {

		class ColoredDrawSystem : public AbstractDrawSystem<ColoredDrawSystem> {
		public:
			ColoredDrawSystem();
			void run(std::vector<ECS::Entity> &);
		private:

			// program id
			GLuint programId;

			// uniforms
			GLuint viewU, projectionU, modelTransformU, tiModelTransformU, useTextureU, textureHW, hwLightDir;
			GLuint isSkeletalU;
		};

	}

}




#endif
