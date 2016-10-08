#pragma once

#ifndef __TACTICS_ACTOR_H__
#define __TACTICS_ACTOR_H__

#include "Component.h"

#include <string>
#include <map>
#include <vector>

namespace Tactics {

	namespace Actor {

		template <typename T>
		std::string toString(const T &);

		enum JobClass {
			Soldier,  // basic fighter type
			Thrasher,  // stronger fighter, high defense, close combat
			Ranger, // basic gun user
			Sniper,  // ranged shooter
			Hitman,  // comby Spy + Sniper
			Medic,  // healer / item user
			Engineer,  // offensive item user
			Grenadier, // explosives
			General,  // very powerful
			Pilot,  // rides bikes, mechs, etc.
			Scout,  // support unit, disrupts enemies
			Spy,  // support unit, more powerful scout
			Magician,  // mage
			Tweaker,  // master of machinery
			Hacker // powerful tweaker
		};

		template <>
		std::string toString(const JobClass & jc) {
			switch (jc) {
			case Soldier: return "soldier";
			case Thrasher: return "thrasher";
			case Ranger: return "ranger";
			case Sniper: return "sniper";
			case Hitman: return "hitman";
			case Medic: return "medic";
			case Engineer: return "engineer";
			case Grenadier: return "grenadier";
			case General: return "general";
			case Pilot: return "pilot";
			case Scout: return "scout";
			case Spy: return "spy";
			case Magician: return "magician";
			case Tweaker: return "tweaker";
			case Hacker: return "hacker";
			}
			return "";
		}

		// special classes used by npcs, monsters, etc
		enum SpecialClass {
			Foreman,
			Worker
		};

		enum Species {
			Human,
			Monster,
			Machine
		};

		template <>
		std::string toString(const Species & s) {
			switch (s) {
			case Human: return "human";
			case Monster: return "monster";
			case Machine: return "machine";
			}
			return "";
		}

		enum Status {
			Normal,
			Bleeding,
			Poison,
			Burning,
			Frozen,
			Blind,
			KO
		};

		template <>
		std::string toString(const Status & s) {
			switch (s) {
			case Normal: return "normal";
			case Bleeding: return "bleeding";
			case Poison: return "poison";
			case Burning: return "burning";
			case Frozen: return "frozen";
			case Blind: return "blind";
			case KO: return "ko";
			}
			return "unknown";
		}

		// return random name from names file
		std::string getRandomName();

		class Item;

		namespace Components {
			
			struct Name : public ECS::Component {
				std::string name;
			};

			struct BaseStats : public ECS::Component {
				std::map<std::string, unsigned short> stats = {
					{"strength",0},
					{"vitality",0},
					{"accuracy",0},
					{"speed",0},
					{"spirit",0},
					{"luck",0},
					{"movement",0}
				};
			};

			struct Armor : public ECS::Component {
				std::map<std::string, unsigned short> stats = {
					{"armor",0}
				};
			};

			struct TileLocation : public ECS::Component {
				unsigned short x;
				unsigned short y;
			};

			struct Status : public ECS::Component {
				Tactics::Actor::Status status;
			};

			struct Level : public ECS::Component {
				unsigned short level;
			};

			struct Inventory : public ECS::Component {
				std::vector<Item *> inventory;
			};

		} // namespace Component





		// generic actor
		class Actor {
		public:

			// equip supplied equipment on this actor
			//void equip(Equipment & e);

		protected:
			int id;
			static int autoid;
			std::string name;
			std::map<std::string, int> numeric_stats;
			std::string classname;
			std::vector<Item *> equipped_items;

		};



	} // namespace Actor

} // namespace Tactics


#endif

