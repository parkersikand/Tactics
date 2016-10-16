
#include <GL/glew.h>

#include "Util.h"

#include "Object3D.h"

double avg(std::vector<double> v) {
	double s = 0;
	for (auto val : v) s += val;
	return s / v.size();
}

char * readFile(const char * filename) {
	FILE * file;
	file = fopen(filename, "r");
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buf = (char *)malloc(length * sizeof(char));
	fread(buf, sizeof(char), length, file);
	fclose(file);
	buf[length - 1] = '\0';
	return buf;
}

void printLog(GLuint obj) {
	int infologLength = 0;
	char infoLog[1024];

	if (glIsShader(obj))
		glGetShaderInfoLog(obj, 1024, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, 1024, &infologLength, infoLog);

	if (infologLength > 0)
		printf("%sn", infoLog);
}

// make a unit cube at the origin
void make_cube(Tactics::Components::CObject3D * ptr) {
	float cubePoints[] = {
	  -1.0, -1.0, -1.0,    -1.0, -1.0, 1.0,    -1.0, 1.0, 1.0,
	  -1.0, -1.0, -1.0,    -1.0, 1.0, -1.0,    -1.0, 1.0, 1.0,

	  -1.0, -1.0, -1.0,    -1.0, -1.0, 1.0,    1.0, -1.0, 1.0,
	  -1.0, -1.0, -1.0,    1.0, -1.0, -1.0,    1.0, -1.0, 1.0,
	  
	  -1.0, -1.0, -1.0,    -1.0, 1.0, -1.0,    1.0, 1.0, -1.0,
	  -1.0, -1.0, -1.0,    1.0, -1.0, -1.0,    1.0, 1.0, -1.0,

	  1.0, 1.0, 1.0,    1.0, 1.0, -1.0,    1.0, -1.0, -1.0,
	  1.0, 1.0, 1.0,    1.0, -1.0, 1.0,    1.0, -1.0, -1.0,
	  
	  1.0, 1.0, 1.0,    1.0, 1.0, -1.0,    -1.0, 1.0, -1.0,
	  1.0, 1.0, 1.0,    -1.0, 1.0, 1.0,    -1.0, 1.0, -1.0,
	  
	  1.0, 1.0, 1.0,    1.0, -1.0, 1.0,    -1.0, -1.0, 1.0,
	  1.0, 1.0, 1.0,    -1.0, 1.0, 1.0,    -1.0, -1.0, 1.0
	};

	std::vector<glm::vec3> vx(3*2*6);
	for (int i = 0, v = 0; i < 3 * 3 * 2 * 6; i += 3, v++) {
		vx[v] = glm::vec3(cubePoints[i], cubePoints[i + 1], cubePoints[i + 2]);
	}

	Tactics::Components::CObject3DHelper::setVertexData(ptr, vx);

} // void make_cube

std::string & quotedString(std::istream & in, std::string & str) {
	if (in.peek() == '"') {
		in.get();
	}
	else {
		std::string discard;
		std::getline(in, discard, '"');
	}
	std::getline(in, str, '"');
	return str;
}


float lineNormalAngleRad(glm::vec3 line, glm::vec3 normal) {
	auto normLine = glm::normalize(line);
	auto normNormal = glm::normalize(normal);
	return glm::asin(glm::length(glm::dot(line, normNormal)) / (glm::length(line) * glm::length(normNormal)));
}

