#pragma once


#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <bitset>

#define MAX_COMPONENTS 1024

namespace Tactics {

	namespace ECS {

		// A component is data. JUST data.
		struct Component {};

		typedef unsigned int ComponentId;

		// IDEA component dependancies

		// compound component
		/*
		struct CompoundComponent : public Component {
			std::map<ComponentId, Component> components;

			template <typename ComponentType>
			Component * getComponent() {
				return &components[ComponentManager::getComponentId<ComponentType>()];
			}
		};
		*/
		

		// Objectification of a component mask, since we support arbitrary numbers of components
		class ComponentMask {
		public:
			
			ComponentMask() {};
			//ComponentMask(std::vector<std::string>);
		
			friend bool operator==(const ComponentMask &, const ComponentMask &);

			bool isSet(int i) { return mask[i]; }
			
			template <typename T>
			bool isSet();

			void set(int i) { mask.set(i); }
			
			template <typename T>
			void set();

			bool includes(ComponentMask other) {
				return (other.mask & mask) == other.mask;
			}

		private:

			// perform initialization of this mask
			void init();

			std::bitset<MAX_COMPONENTS> mask;

			friend class MaskBuilder;
			friend class ComponentManager;
		};

		class MaskBuilder {
		public:
			MaskBuilder() {};

			template <typename ComponentType>
			MaskBuilder set() {
				_mask.set<ComponentType>();
				return *this;
			}

			ComponentMask mask() const { return _mask; }
		private:
			ComponentMask _mask;
		};

		// Class to manage registration of components
		// Componenets are useless until they are registered
		class ComponentManager {
		public:

			ComponentManager() {};
			~ComponentManager();

			static ComponentManager * getInstance() {
				if (!instance) {
					instance = new ComponentManager;
				}
				return instance;
			}

			template <typename T>
			int registerComponent();

			// returns number of registered components
			unsigned int size() const { return counter - 1; }

			static unsigned int getSize() {
				return instance->counter - 1;
			}

			Component * lookupPrototypeById(ComponentId id) {
				return prototypes[ids_tags[id]];
			}

			Component * lookupPrototypeByTag(std::string tag) {
				return prototypes[tag];
			}

			template <typename T>
			Component * lookupPrototype() {
				return lookupPrototypeByTag(T::tag());
			}

			// use template magic to simultaneously register and index components
			template <typename T>
			static ComponentId getComponentId() {
				static ComponentId cid = 0;
				if (cid > 0) return cid;
				cid = getInstance()->counter;
				getInstance()->counter++;
				// TODO update component mask
				return cid;
			}

		private:

			static ComponentManager * instance;

			// serves as an ID and size counter for components
			unsigned int counter = 1;

			// whether we are accepting new registration or not
			bool closed = false;

			// map of component tags to their ids
			std::map<std::string, int> tags_ids;

			// map of ids to tags
			std::map<int, std::string> ids_tags;

			// store a prototype of each component
			std::map<std::string, Component *> prototypes;
		};


		bool operator==(const ComponentMask &, const ComponentMask &);


		// Implementation of template functions
		template <typename T>
		inline bool ComponentMask::isSet() {
			return mask[ComponentManager::getComponentId<T>()];
		}

		template <typename T>
		inline void ComponentMask::set() {
			mask.set(ComponentManager::getComponentId<T>());
		}


	} // namespace ECS

} // namespace Tactics







#endif




