
#include "ParticleSystem.h"

#include "DrawSystem.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

using namespace Tactics;

ECS::EntityHdl Systems::ParticleSystem::emitParticle() {
	ECS::World & world = *getWorld();
	ECS::EntityHdl particle = world.newEntity();
	auto * particleC = world.addComponent<Components::Particle>(particle);
	particleC->birthTime = glfwGetTime();
	particleC->color = glm::vec4(0.f, 0.f, 0.f, 0.f);
	particleC->particleSystem = this;
	return particle;
}

void Systems::ParticleDrawSystem::run(std::vector<ECS::Entity> & entities) {
	glUseProgram(programId);

}
