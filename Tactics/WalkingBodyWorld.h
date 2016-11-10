#pragma once


#include "BasicWorld.h"
#include "KeyInputSystem.h"
#include "Object3D.h"
#include "SkeletalAnimation.h"
#include "System.h"

class WalkingBodyWorld : public Tactics::Worlds::BasicWorld {
public:
	WalkingBodyWorld();
	void setup();

	struct BodyUpdater : public virtual Tactics::ECS::RunnableSystem, 
		                 public virtual Tactics::Systems::KeyInputSystem {
		void run();

		Tactics::Components::ModelTransform * modelTransform;
		Tactics::Components::SkeletalAnimationController * animationController;
		Tactics::Components::Position3D<> * position;

		float facing = 0.f;
		float walkSpeed = 0.001f;
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
