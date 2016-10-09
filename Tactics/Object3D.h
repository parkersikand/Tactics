#pragma once

#ifndef __3D_OBJ_H__
#define __3D_OBJ_H__

#include <GL/glew.h>

#include "GLFW/glfw3.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Component.h"

#include <vector>

namespace Tactics {

	namespace Components {

		struct CObject3D : public ECS::Component {
			CObject3D();
			CObject3D(const std::vector<glm::vec3> &, const std::vector<glm::vec2> &, const std::vector<glm::vec3> &);
			
			GLuint vxBufId;
			std::vector<glm::vec3> vxData;
			GLuint uvBufId;
			std::vector<glm::vec2> uvData;
			GLuint normBufId;
			std::vector<glm::vec3> normData;

			// index data
			GLuint idxBufId; 
			std::vector<unsigned int> idxData;

			GLuint texId;
			GLuint count;
			unsigned int drawMode = GL_TRIANGLES;
		};

		struct CObject3DHelper {
			static void setVertexData(CObject3D *, std::vector<glm::vec3> &);
			static void setUVData(CObject3D *, std::vector<glm::vec2> &);
			static void setNormData(CObject3D *, std::vector<glm::vec3> &);
			static void setData(CObject3D *, const std::vector<glm::vec3> &, const std::vector<glm::vec2> &, const std::vector<glm::vec3> &);
			static bool loadTexture2Dpng(CObject3D *, const char *, bool flipImage = false);
			static void loadTexture2D(CObject3D *, unsigned int, unsigned int, void *, GLenum = GL_RGBA, GLenum = GL_UNSIGNED_BYTE);
		};


		// Transformations on an objects points
		// Used primarily to scale or rotate an object
		struct ModelTransform : public ECS::Component {
			// Model transform
			// default to identity
			glm::mat4 transform;
		};


		// Position an object in 3d space
		template <typename ValType = GLfloat>
		struct Position3D : public ECS::Component {
			ValType x = 0;
			ValType y = 0;
			ValType z = 0;
		};

		// Colors for vertices
		struct Colored3D : public ECS::Component {
			std::vector<glm::vec3> data;
			GLuint bufId = 0;
		};

		// Helper functions
		struct Colored3DHelper {
			static void SingleColor(Colored3D *, CObject3D *, glm::vec3 color = glm::vec3(0.f, 0.f, 0.f));
		};

	} // namespace Components

} // namespace Tactics



#endif
