#pragma once

#ifndef __TACTICS_MATERIAL_H__
#define __TACTICS_MATERIAL_H__


#include "Component.h"

#include <glm.hpp>

namespace Tactics {

	namespace Components {

		struct Material : public ECS::Component {
			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;
		};

	} // namespace Components

} // namespace Tactics




#endif
