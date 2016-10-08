#pragma once

#ifndef __TACTICS_MODEL_STORE_H__
#define __TACTICS_MODEL_STORE_H__

#include "OBJLoader.h"
#include "Object3D.h"

#include <string>
#include <vector>
#include <map>

namespace Tactics {

	class ModelStoragePolicy; 

	// manages loading and storing models
	class ModelStore {
	public:
		~ModelStore();

		ModelStore(ModelStoragePolicy &) {};

		ModelStore & getInstance() {
			if (instance == nullptr) {

			}
		}

		static void LoadAndStoreNamedModel(char * fname, char * name = NULL);

		class Object {
		public:

			std::string name;

		private:

		};

	private:
		static ModelStore * instance;

	};

	class ModelStoragePolicy; {
	public:

	private:

	};


}


#endif