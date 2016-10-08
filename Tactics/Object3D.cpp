#include <GL/glew.h>

#include "Component.h"
#include "Object3D.h"

#include "lodepng.h"

#include <glm.hpp>

#include <iostream>
#include <iomanip>


using namespace Tactics;
using namespace Tactics::ECS;

// Default constructor, initialize empty buffers

Components::CObject3D::CObject3D() {
	glGenBuffers(1, &vxBufId);
	glGenBuffers(1, &uvBufId);
	glGenBuffers(1, &normBufId);
	glGenBuffers(1, &idxBufId);

	texId = 0; // By default, set texture to 0 (null)
} // CObject3D()


// Create buffers with data
Components::CObject3D::CObject3D(const std::vector<glm::vec3> & vs, const std::vector<glm::vec2> & uvs, const std::vector<glm::vec3> & norms) {
	glGenBuffers(1, &vxBufId);
	glGenBuffers(1, &uvBufId);
	glGenBuffers(1, &normBufId);
	glGenBuffers(1, &idxBufId);

	texId = 0;
	Components::CObject3DHelper::setData(this, vs, uvs, norms);
} 

void Components::CObject3DHelper::setData(CObject3D * obj, const std::vector<glm::vec3> & vs, const std::vector<glm::vec2> & uvs, const std::vector<glm::vec3> & norms) {
	if (vs.size() > 0) {
		obj->vxData = std::vector<glm::vec3>(vs);
		glBindBuffer(GL_ARRAY_BUFFER, obj->vxBufId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vs[0]) * vs.size(), &vs[0], GL_STATIC_DRAW);
	}
	if (uvs.size() > 0) {
		obj->uvData = std::vector<glm::vec2>(uvs);
		glBindBuffer(GL_ARRAY_BUFFER, obj->uvBufId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uvs[0]) * uvs.size(), &uvs[0], GL_STATIC_DRAW);
	}
	if (norms.size() > 0) {
		obj->normData = std::vector<glm::vec3>(norms);
		glBindBuffer(GL_ARRAY_BUFFER, obj->normBufId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(norms[0]) * norms.size(), &norms[0], GL_STATIC_DRAW);
	}
	obj->count = vs.size();
}

void Components::CObject3DHelper::setVertexData(CObject3D * obj, std::vector<glm::vec3> & data) {
	obj->vxData = std::vector<glm::vec3>(data);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vxBufId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data[0])*data.size(), &data[0], GL_STATIC_DRAW);
	obj->count = data.size();
}

void Components::CObject3DHelper::setUVData(CObject3D * obj, std::vector<glm::vec2> & data) {
	obj->uvData = std::vector<glm::vec2>(data);
	glBindBuffer(GL_ARRAY_BUFFER, obj->uvBufId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data[0])*data.size(), &data[0], GL_STATIC_DRAW);
}

void Components::CObject3DHelper::loadTexture2D(CObject3D * obj, unsigned int width, unsigned int height, void * data, GLenum fmt, GLenum type) {
	glGenTextures(1, &obj->texId);
	glBindTexture(GL_TEXTURE_2D, obj->texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, fmt, type, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // not necessary
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // not necessary
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // not necessary
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // necessary
}

bool Components::CObject3DHelper::loadTexture2Dpng(CObject3D * obj, const char * fname, bool flipImage) {
	std::vector<unsigned char> data;
	unsigned int width, height;
	void * ptr;

	double time = glfwGetTime();
	unsigned error = lodepng::decode(data, width, height, fname);
	std::cout << "PNG load " << std::setprecision(2) << (glfwGetTime() - time)*1000 << "ms" << std::endl;
	if (error) {
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return false;
	}
	ptr = &data[0];
	
	if (flipImage) {

		std::vector<unsigned char> tmp(data.size());

		// flip image vertically
		time = glfwGetTime();
		unsigned int width_bytes = width * 4;

		// this is slow, try not to do this
		for (unsigned int i = 0; i < height; i++) {
			for (unsigned int j = 0; j < width_bytes; j++) {
				tmp[i * width_bytes + j] = data[(height - i - 1)*width_bytes + j];
			}
		}
		std::cout << "Flip image " << std::setprecision(2) << (glfwGetTime() - time) * 1000 << "ms" << std::endl;
		ptr = &tmp[0];
	}
	
	loadTexture2D(obj, width, height, ptr, GL_RGBA, GL_UNSIGNED_BYTE);
	
	return true;

} // bool loadTexture2Dpng(CObject3D * obj, const char * fname, bool flipImage = false)

void Components::Colored3DHelper::SingleColor(Colored3D * col3d, CObject3D * obj, glm::vec3 color) {
	if (col3d->bufId == 0) { // create a buffer if one does not exist
		glGenBuffers(1, &col3d->bufId);
	}
	glBindBuffer(GL_ARRAY_BUFFER, col3d->bufId);
	col3d->data = std::vector<glm::vec3>(obj->count, color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(col3d->data[0]) * col3d->data.size(), &col3d->data[0], GL_STATIC_DRAW);
}
