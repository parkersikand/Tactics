
#include <GL/glew.h>

#include "Util.h"

#include "Object3D.h"

#include <limits>
#include <iomanip>


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
	return (glm::pi<float>() / 2) - glm::asin(glm::length(glm::dot(line, normNormal)) / (glm::length(line) * glm::length(normNormal)));
}


// naive, fast implementation
float e = 10e-7f;
bool eqf(float a, float b) {
	return abs(a - b) < e;
}


ostream & operator<<(ostream & out, const Matrix4f & m) {
	out << std::fixed << setprecision(8);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out << m.m[i][j];
			if (j < 3) out << '\t';
		}
		out << endl;
	}
	return out;
}


glm::mat4 Matrix4f2mat4(Matrix4f mat) {
	glm::mat4 out;
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			out[i][j] = mat.m[i][j];
		}
	}
	return out;
}


bool mateq(Matrix4f a, glm::mat4 b) {
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			if (!eqf(a.m[i][j], b[i][j])) return false;
		}
	}
	return true;
}

float glfwGetTimef() {
	return (float)glfwGetTime();
}


vector<glm::vec3> make_square_tris(float size) {
	vector<glm::vec3> out;
	out.push_back(glm::vec3(-size / 2, -size / 2, 0));
	out.push_back(glm::vec3(-size / 2, size / 2, 0));
	out.push_back(glm::vec3(size / 2, size / 2, 0));
	out.push_back(glm::vec3(size / 2, size / 2, 0));
	out.push_back(glm::vec3(size / 2, -size / 2, 0));
	out.push_back(glm::vec3(-size / 2, -size / 2, 0));
}
