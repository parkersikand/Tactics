
#include "GuideLines.h"

#include "Object3D.h"
#include "BasicDrawSystem.h"

using namespace Tactics;
using namespace Tactics::ECS;

Entities::GuideLines::GuideLines(World & w) : Entity(w) {
	EntityHdl lines = newChild();
	w.addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem>>(lines);
	std::vector<glm::vec3> vxs = {
		glm::vec3(-100.f, 0.f, 0.f) , glm::vec3(100.f, 0.f, 0.f),
		glm::vec3(0.f, -100.f, 0.f), glm::vec3(0.f, 100.f, 0.f),
		glm::vec3(0.f, 0.f, -100.f), glm::vec3(0.f, 0.f, 100.f)
	};
	auto * lobj = w.addComponent<Components::CObject3D>(lines);
	Components::CObject3DHelper::setVertexData(lobj, vxs);
	lobj->drawMode = GL_LINES;
	auto * col = w.addComponent<Components::Colored3D>(lines);
	Components::Colored3DHelper::SingleColor(col, lobj, glm::vec3(0.0));
	auto * pos = w.addComponent<Components::Position3D<>>(lines);
}
