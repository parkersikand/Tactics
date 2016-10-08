#pragma once

#include "GLVector.h"

using Tactics::Util::GLVector;

GLVector::GLVector() {
	currentSize = 0;
	currentCapacity = 1024;
	glGenBuffers(1, &vbo);
	// reserve space
	glBindBuffer(bufferTarget, vbo);
	glBufferData(bufferTarget, currentSize, NULL, usage);
	//unbind
	glBindBuffer(bufferTarget, 0);
}

bool GLVector::insert(size_t size, void * data) {
	if (currentSize + size > currentCapacity) {
		if (!resize(currentCapacity, 2 * currentCapacity))
			return false;
	}
	glBindBuffer(bufferTarget, vbo);
	glBufferSubData(bufferTarget, currentSize, size, data);
	glBindBuffer(bufferTarget, 0);
	return true;
}

bool GLVector::resize(size_t oldsize, size_t newsize) {
	// generate and reserve the new buffer
	GLuint tmpVBO;
	glGenBuffers(1, &tmpVBO);
	glBindBuffer(bufferTarget, tmpVBO);
	glBufferData(bufferTarget, newsize, NULL, usage);
	glBindBuffer(bufferTarget, 0);
	// set up copy buffers
	glBindBuffer(GL_COPY_READ_BUFFER, vbo);
	glBindBuffer(GL_COPY_WRITE_BUFFER, tmpVBO);
	// copy data
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, currentSize);
	return false;
}
