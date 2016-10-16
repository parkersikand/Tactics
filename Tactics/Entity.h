#pragma once


#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "Component.h"
#include "Constants.h"

#include <vector>
#include <set>

namespace Tactics {

	namespace ECS {

		class World;
		class System;
		class RunnableSystem;

		typedef unsigned int EntityHdl;

		// An entity is a composition of Components
		// TODO allow construct to create/manage child entities. Currently, they get orphaned.
		struct Entity {
			
			// no default constructor, since an entity is meaningless without a world
			Entity(World &);

			ComponentMask getMask() const { return *mask; };
			EntityHdl getHandle() const { return hdl; }

			template <typename ComponentType>
			ComponentType * getComponent() {
				return world->getComponent<ComponentType>(hdl);
			}

			template <typename ComponentType>
			ComponentType * addComponent() {
				return world->addComponent<ComponentType>(hdl);
			}

			template <typename ComponentType>
			ComponentType * addComponent(ComponentType c) {
				return world->addComponent<ComponentType>(hdl, c);
			}

			// Remove a component from this entity
			template <typename ComponentType>
			void removeComponent() {
				world->removeComponent<ComponentType>(hdl);
			}

			// add a child entity
			EntityHdl newChild();

			// destroy this entity and children
			void destroy();

		protected:
			// the world we belong to
			World * world = NULL;

			// pointer to the masked stored in the world
			// if mask is NULL, the entity is invalid
			ComponentMask * mask = NULL;

			// our handle in the world
			EntityHdl hdl = 0;

			friend class World;
			Entity(World * w, ComponentMask * m, EntityHdl h) : world(w), mask(m), hdl(h) {};
		};
		

		// The world is the container of entities
		// stores the actual components
		class World {
		public:
			~World();

			World();

			EntityHdl addEntity(Entity &);

			template <typename T>
			T * getComponent(EntityHdl hdl) {
				if(container[ComponentManager::getComponentId<T>()][hdl])
					return static_cast<T*>(container[ComponentManager::getComponentId<T>()][hdl]);
				return nullptr;
			}

			// add a new component onto a registered entity
			template <typename T>
			T * addComponent(EntityHdl hdl) {
				return addComponent(hdl, T());
			}

			template <typename T>
			T * addComponent(EntityHdl hdl, T t) {
				Component ** ptr = &container[ComponentManager::getComponentId<T>()][hdl];
				if (*ptr) delete (*ptr);
				*ptr = new T(t);
				mask_arr[hdl].set(ComponentManager::getComponentId<T>());
				return static_cast<T*>(*ptr);
			}

			template <typename ComponentType>
			void removeComponent(EntityHdl hdl) {
				ComponentId id = ComponentManager::getComponentId<ComponentType>();
				if (container[id][hdl] != nullptr) {
					delete container[id][hdl];
					container[id][hdl] = nullptr;
				}
			}

			// Return a handle to a new blank entity
			EntityHdl newEntity() {
				idx++;
				return idx - 1;
			}

			template <typename EntityType>
			EntityHdl newTypedEntity() {
				EntityType e(*this);
				return e.getHandle();
			}

			// Return handles that match this mask exactly
			std::vector<EntityHdl> filterMaskExact(ComponentMask mask) {
				std::vector<EntityHdl> out;
				for (unsigned int i = 0; i < idx; i++) {
					if (mask == mask_arr[i]) out.push_back(i);
				}
				return out;
			}

			// Return handles that have all required bits set
			std::vector<EntityHdl> filterMaskIncludes(ComponentMask mask) {
				std::vector<EntityHdl> out;
				for (unsigned int i = 0; i < idx; i++) {
					if (mask_arr[i].includes(mask)) out.push_back(i);
				}
				return out;
			}

			// Returns Entity structs created from their handles
			std::vector<Entity> entitiesFromHandles(std::vector<EntityHdl> handles) {
				std::vector<Entity> es;
				for (auto hdl : handles) {
					es.push_back(Entity(this, &mask_arr[hdl], hdl));
				}
				return es;
			}

			// Get one Entity
			Entity entityFromHandle(EntityHdl hdl) {
				return Entity(this, &mask_arr[hdl], hdl);
			}

			// register a system
			void registerSystem(System & system);

			// add a system to list of runnable systems
			void addSystem(System & system);

			// run all registered systems
			void runAllSystems();

			// add a global system
			template <typename SystemType>
			void addGlobalSystem(SystemType & system) {
				int id = World::lookupGlobalSystemId<SystemType>();
				if (globalSystems[id]) {
					delete globalSystems[id];
				}
				registerSystem(system);
				globalSystems[id] = &system;
			};

			template <typename SystemType>
			SystemType * getGlobalSystem() {
				SystemId id = lookupGlobalSystemId<SystemType>();
				return dynamic_cast<SystemType *>(globalSystems[id]);
			};

			// Create a managed instance of a system. Instance can then be added as global, runnable, etc
			template <typename SystemType>
			SystemType * createManagedSystem() {
				auto * ptr = new SystemType();
				managedSystems.push_back(ptr);
				registerSystem(*ptr);
				return ptr;
			}

			void destroyManagedSystem(System *);

			// remove an entity from the world
			void removeEntity(EntityHdl);

			// create child
			EntityHdl createChildEntity(EntityHdl parent);

			// set parent-child relationship
			void setParentChild(EntityHdl parent, EntityHdl child);

			// virtual methods
			virtual void setup() {};

		private:
			unsigned int starting_size = 1024; // allocate space for 1024 entities

			unsigned int current_size = starting_size;

			// The index of the next available entity
			unsigned int idx = 1;

			// TODO implement these
			void resize();
			void grow();
			void shrink();

			// dynamic array of masks
			ComponentMask * mask_arr;

			// map of component keys to arrays of component pointers
			// pointers are nullptr for entities without a certain component
			Component **container[ECS_MAX_COMPONENTS];

			// systems

			// vanilla systems container. not managed.
			std::vector<System *> systems;

			// managed systems. we will delete these in the destructor
			std::vector<System *> managedSystems;

			typedef int SystemId;

			// function to lookup global system by type
			template <typename SystemType>
			SystemId lookupGlobalSystemId() {
				static unsigned int id;
				if (id > 0) return id;
				id = _global_system_counter;
				_global_system_counter++;
				return id;
			}
			unsigned int _global_system_counter = 1;

			// global systems. can only be one of a given type
			System *globalSystems[ECS_MAX_SYSTEM_TYPES] = { NULL };

			// Entity hierarchy
			std::map<EntityHdl, std::set<EntityHdl>> entityHierarchy;
		};


		// A world with a main loop
		class RunnableWorld : public World {
		public:

			// set up the world, add systems, entities, etc.
			virtual void setup() {};

			// cleanup
			virtual void shutdown() {};

			// start the run loop
			void start();

			// must be called from a system within the world
			void stop();

			// adds a runnable system
			void addRunnableSystem(RunnableSystem & s);

			template <typename RunnableSystemType>
			void addRunnableGlobalSystem(RunnableSystemType & s) {
				addGlobalSystem(s);
				systems.push_back(&s);
				registerRunnableSystem(s);
			}

			void registerRunnableSystem(RunnableSystem &);

		protected:
			void run();
		private:
			bool running = false;
			std::vector<RunnableSystem*> systems;
		};

	} // namespace ECS

} // namespace Tactics




#endif
