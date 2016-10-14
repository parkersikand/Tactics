

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include <iostream>

using namespace std;
using namespace glm;

ostream & operator<<(ostream & out, const mat4 & m) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out << m[i][j] << " ";
		}
		out << endl;
	}
	return out;
}

int main(int argc, char **argv) {

	mat4 id;
	cout << id << endl;

	// rotate 90 on X
	cout << "Rotate 90 on X:" << endl;
	cout << glm::rotate(mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)) << endl;

	cout << "Rotate 90 on Y:" << endl <<
		glm::rotate(mat4(1.f), glm::radians(90.f), vec3(0.f, 1.f, 0.f)) << endl;
	
	cout << "Rotate 90 on X and Y:" << endl <<
		glm::rotate(glm::rotate(mat4(1.f), glm::radians(90.f), vec3(1.f, 0.f, 0.f)), glm::radians(90.f), vec3(0.f, 1.f, 0.f)) << endl;

	getchar();
}



