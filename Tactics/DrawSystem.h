#pragma once


#ifndef __TACTICS_DRAWSYSTEM_H__
#define __TACTICS_DRAWSYSTEM_H__

#include "System.h"
#include "Event.h"
#include "Component.h"

//#include "TimedFiringSystem.h"
//#include "EventedSystem.h"
//#include "Object3D.h"


namespace Tactics {

	/*
	struct DrawEvent : public ECS::Event {};

	namespace Components {

		// Anything that can be drawn
		struct Drawable : public ECS::Component {};

	} // namespace Components


	// Runnable system that fires a draw event every 30 ms
	struct DrawEventShooter : public TimedFiringSystem<DrawEvent> {
		DrawEventShooter() : TimedFiringSystem<DrawEvent>(30.0) {};
	};
	*/

	/*
	// Global system that contains child drawing systems
	class DrawSystem : public virtual ECS::IncludesFilteredSystem, public virtual EventedSystem<DrawEvent> {
	public:

		DrawSystem() : ECS::IncludesFilteredSystem(ECS::MaskBuilder().set<Components::Drawable>().mask()) {};
		DrawSystem(GLFWwindow * w) : ECS::IncludesFilteredSystem(ECS::MaskBuilder().set<Components::Drawable>().mask()), myWindow(w) {};
		DrawSystem(GLFWwindow * w, GLuint p) : ECS::IncludesFilteredSystem(ECS::MaskBuilder().set<Components::Drawable>().mask()),
			myWindow(w),
			programId(p) {};

		// called when DrawEvent is fired
		virtual void run(std::vector<ECS::Entity> &);
	
		void setProgram(GLuint p) {
			programId = p;
			glUseProgram(programId);
		}

		GLuint getProgramId() const { return programId; }

	protected:
		GLFWwindow * getWindow() { return myWindow; }

	private:

		// ID of the current shader program
		GLuint programId;

		// The window associated with the drawing system
		GLFWwindow * myWindow;

		// sub drawing systems
		std::vector<DrawSystem *> systems;

	}; // class DrawSystem


	template <>
	inline Components::Drawable * ECS::World::addComponent<Components::Drawable>(ECS::EntityHdl hdl) {
		addComponent(hdl, Components::Position3D<>());
		addComponent(hdl, Components::CObject3D());
		addComponent(hdl, Components::ModelTransform());
		return addComponent(hdl, Components::Drawable());
	}
	*/

	namespace Components {

		// Tag component specifying that this entity should be drawn by DrawSystemType
		template <typename DrawSystemType>
		struct DrawSystemTag : public ECS::Component {};

	} // namespace Components

	namespace Systems {

		// For internal use only
		struct BaseAbstractDrawSystem : public ECS::IncludesFilteredSystem {
			using ECS::IncludesFilteredSystem::IncludesFilteredSystem;
			void onRegister(ECS::World &);
			//ECS::EntityHdl createEntity();
		};

		template <typename DrawSystemType>
		struct AbstractDrawSystem : public BaseAbstractDrawSystem {
			AbstractDrawSystem() : BaseAbstractDrawSystem(ECS::MaskBuilder().set<Components::DrawSystemTag<DrawSystemType> >().mask()) {};
		};

	} // namespace Tactics::Systems

} // namespace Tactics


#endif
