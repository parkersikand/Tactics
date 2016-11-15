#pragma once


#include "BasicWorld.h"
#include "KeyInputSystem.h"
#include "Object3D.h"
#include "SkeletalAnimation.h"
#include "System.h"
#include "EventedSystem.h"

class BodyUpdateEvent : public Tactics::ECS::Event {};

class WalkingBodyWorld : public Tactics::Worlds::BasicWorld {
public:
	WalkingBodyWorld();
	void setup();

	struct BodyUpdater : public virtual Tactics::EventedSystem<BodyUpdateEvent>, 
		                 public virtual Tactics::Systems::KeyInputSystem {
		//void run();
		void handle(const BodyUpdateEvent &);

		Tactics::Components::ModelTransform * modelTransform;
		Tactics::Components::SkeletalAnimationController * animationController;
		Tactics::Components::Position3D<> * position;
		Tactics::Components::BillboardVarying * billboard;

		float facing = 0.f;
		float walkSpeed = 0.1f;
		bool walking = false;
		float angularVelocity = 0.f;

		glm::mat4 baseTransform;

		void handle(const Tactics::Events::KeyDown &);
		void handle(const Tactics::Events::KeyUp &);
	};


private:
	BodyUpdater updater;
	Tactics::ECS::EntityHdl bodyHdl;
};
