

#include "OBJLoader.h"
#include "Object3D.h"

#include <cstdio>
#include <stdio.h>
#include <regex>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


// stream based implementation, very fast
bool LoadObj(const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	bool flipUVs) {

	out_vertices.clear();
	out_uvs.clear();
	out_normals.clear();

	std::ifstream in(path);
	if (!in) {
		std::cout << "Could not open file \"" << path << "\"" << std::endl;
		return false;
	}

	std::string line;
	std::vector<glm::vec3> vertices, normals;
	std::vector<glm::vec2> uvs;

	std::istringstream lineStream;
	std::string head;
	
	// read line by line
	while (true) {
		if (in.peek() == 'f') break;
		getline(in, line);
		lineStream = std::istringstream(line);
		lineStream >> head;
		
		if (head == "v") {
			glm::vec3 v;
			lineStream >> v.x >> v.y >> v.z;
			vertices.push_back(v);
		}
		else if (head == "vt") {
			glm::vec2 uv;
			lineStream >> uv.x >> uv.y;
			if (flipUVs) uv.y = 1.f - uv.y;
			uvs.push_back(uv);
		}
		else if (head == "vn") {
			glm::vec3 n;
			lineStream >> n.x >> n.y >> n.z;
			normals.push_back(n);
		}

	} // while getline

	// switch how we read faces based on presence of uvs
	char discard;
	int vi, uvi, ni;
	std::vector<glm::ivec3> faces;

	if (uvs.empty()) {
		while (in >> discard && discard == 'f') {
			for (int i = 0; i < 3; i++) {
				in >> vi >> discard >> discard >> ni;
				faces.push_back({ vi, -1, ni });
			}
		}
		// copy faces
		out_vertices.resize(faces.size());
		out_normals.resize(faces.size());
		for (unsigned int i = 0; i < faces.size(); i++) {
			out_vertices[i] = vertices[faces[i].x - 1];
			out_normals[i] = normals[faces[i].z - 1];
		}
	} else {
		while (in >> discard && discard == 'f') {
			for (int i = 0; i < 3; i++) {
				in >> vi >> discard >> uvi >> discard >> ni;
				faces.push_back({ vi, uvi, ni });
			}
		}
		// copy faces
		out_vertices.resize(faces.size());
		out_normals.resize(faces.size());
		out_uvs.resize(faces.size());
		for (unsigned int i = 0; i < faces.size(); i++) {
			out_vertices[i] = vertices[faces[i].x - 1];
			out_normals[i] = normals[faces[i].z - 1];
			out_uvs[i] = uvs[faces[i].y - 1];
		}
	} // if uvs.empty()
	
	std::cout << "Read " << out_vertices.size() << " vertices" << std::endl;
	std::cout << "Read " << out_uvs.size() << " uvs" << std::endl;
	std::cout << "Read " << out_normals.size() << " normals" << std::endl;

	return true;
}


bool LoadObj(Tactics::Components::CObject3D * obj, const char * filename) {
	std::vector<glm::vec3> vx, norm;
	std::vector<glm::vec2> uv;
	LoadObj(filename, vx, uv, norm);
	Tactics::Components::CObject3DHelper::setData(obj, vx, uv, norm);
	return true;
}
