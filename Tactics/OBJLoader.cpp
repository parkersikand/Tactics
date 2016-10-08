

#include "OBJLoader.h"

#include <cstdio>
#include <stdio.h>
#include <regex>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// TODO this is horribly slow
/*
bool LoadObj(const char * path,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals,
	bool flipUVs) {

	std::vector<glm::vec3> tmp_vertices;
	std::vector<glm::vec2> tmp_uvs;
	std::vector<glm::vec3> tmp_normals;

	FILE * file = fopen(path, "rb");
	if (file == NULL) {
		std::cout << "Could not open file \"" << path << "\"" << std::endl;
		return false;
	}

	char lineHead[128];
	int res = 0;
	while (res != EOF) {
		res = fscanf(file, "%s", lineHead);

	    if (strcmp(lineHead, "v") == 0) { // vertex
			float x, y, z;
			fscanf(file, "%f %f %f", &x, &y, &z);
			tmp_vertices.push_back(glm::vec3(x, y, z));
		}
		else if (strcmp(lineHead, "vn") == 0) {
			float x, y, z;
			fscanf(file, "%f %f %f", &x, &y, &z);
			tmp_normals.push_back(glm::vec3(x, y, z));
		}
		else if (strcmp(lineHead, "vt") == 0) {
			float u, v;
			fscanf(file, "%f %f", &u, &v);
			tmp_uvs.push_back(glm::vec2(u, v));
		}
		else if (strcmp(lineHead, "f") == 0) { // face
			int v1, uv1, n1, v2, uv2, n2, v3, uv3, n3;

			char buf[1024];
			fgets(buf, sizeof(buf), file);

			if (std::regex_search(buf, std::regex("(\\d+)/(\\d+)/(\\d+)"))) {
				sscanf(buf, "%d/%d/%d %d/%d/%d %d/%d/%d",
					&v1, &uv1, &n1, &v2, &uv2, &n2, &v3, &uv3, &n3);
				if (flipUVs) {
					glm::vec2 tmpUV = tmp_uvs[uv1 - 1];
					tmpUV.y = 1.f - tmpUV.y;
					out_uvs.push_back(tmpUV);
					tmpUV = tmp_uvs[uv2 - 1];
					tmpUV.y = 1.f - tmpUV.y;
					out_uvs.push_back(tmpUV);
					tmpUV = tmp_uvs[uv3 - 1];
					tmpUV.y = 1.f - tmpUV.y;
					out_uvs.push_back(tmpUV);
				}
				else {
					out_uvs.push_back(tmp_uvs[uv1 - 1]);
					out_uvs.push_back(tmp_uvs[uv2 - 1]);
					out_uvs.push_back(tmp_uvs[uv3 - 1]);
				}
				
			}
			else if (std::regex_search(buf, std::regex("(\\d+)//(\\d+)"))) {
				sscanf(buf, "%d//%d %d//%d %d//%d",
					&v1, &n1, &v2, &n2, &v3, &n3);
			}
			else {
				printf("Could not parse line:\n%s\n", buf);
			}

			out_vertices.push_back(tmp_vertices[v1 - 1]);
			out_vertices.push_back(tmp_vertices[v2 - 1]);
			out_vertices.push_back(tmp_vertices[v3 - 1]);

			out_normals.push_back(tmp_normals[n1 - 1]);
			out_normals.push_back(tmp_normals[n2 - 1]);
			out_normals.push_back(tmp_normals[n3 - 1]);
		}
	}

	std::cout << "Read " << out_vertices.size() << " vertices" << std::endl;
	std::cout << "Read " << out_uvs.size() << " uvs" << std::endl;
	std::cout << "Read " << out_normals.size() << " normals" << std::endl;

	return true;
}
*/

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


