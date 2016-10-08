#pragma once


#ifndef __ITEM_H__
#define __ITEM_H__

#include "Actor.h"

namespace Tactics {

	// generic item
	// takes an inventory slot
	// has a value
	class Item {
	public:

	private:
		int baseCost = 0;
		std::string name;
	};

	class Equipment : public Item {
	public:
		
		// equip this item onto an actor
		virtual void equipOn(Actor &);

		// can supplied class use this item
		virtual bool canUse(JobClass) { return true; };

		// returns stat modifiers
		virtual const std::map<std::string, int> getStatModifications();


	protected:

	private:

	};

}








#endif
