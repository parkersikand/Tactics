#pragma once

#ifndef __TACTICS_GLVECTOR_H__
#define __TACTICS_GLVECTOR_H__


#include <GL/glew.h>

#include <vector>

namespace Tactics {

	namespace Util {

		/**
		class GLVector
		  Similar to std::vector, but manages a GL VBO
		  By default, each "element" is 1 byte
		*/
		class GLVector {
		public:

			GLVector();

			// insert data
			bool insert(size_t, void *);

		private:
			GLuint vbo;
			size_t currentSize;
			size_t currentCapacity;
			const size_t MAX_CAPACITY = 65535;

			bool resize(size_t, size_t);

			GLenum bufferTarget = GL_ARRAY_BUFFER;
			GLenum usage = GL_STATIC_DRAW;
			GLenum error;
		};

	} // namespace Tactics::Util

} // naemspace Tactics




#endif
