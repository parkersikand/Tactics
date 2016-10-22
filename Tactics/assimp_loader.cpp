
#include "assimp_loader.h"

#include <iostream>

using namespace std;
using namespace Tactics;

bool LoadMesh(Tactics::Components::CObject3D * obj, const char * filename) {

	Assimp::Importer importer;

	const aiScene * scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (!scene) {
		cout << importer.GetErrorString() << endl;
		return false;
	}

	const aiMesh * mesh = scene->mMeshes[0];

	obj->vxData.clear();
	obj->uvData.clear();
	obj->normData.clear();

	vector<glm::vec3> tmpVertices(mesh->mNumVertices);
	vector<glm::vec3> tmpNormals(mesh->mNumVertices);
	vector<glm::vec2> tmpUVs(mesh->mNumVertices);

	// read vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		const aiVector3D * pPos = &mesh->mVertices[i];
		tmpVertices[i] = glm::vec3(pPos->x, pPos->y, pPos->z);
		
		if (mesh->HasNormals()) {
			tmpNormals[i] = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		}

		if (mesh->HasTextureCoords(0)) {
			tmpUVs[i] = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
	}

	// read faces
	vector<unsigned int> idxs(mesh->mNumFaces * 3);
	for (unsigned int i = 0, f = 0; f < mesh->mNumFaces; f++, i += 3) {
		idxs[i] = mesh->mFaces[f].mIndices[0];
		idxs[i + 1] = mesh->mFaces[f].mIndices[1];
		idxs[i + 2] = mesh->mFaces[f].mIndices[2];
	}

	obj->vxData.resize(mesh->mNumFaces * 3);
	for (unsigned int i = 0; i < idxs.size(); i++) {
		obj->vxData[i] = tmpVertices[idxs[i]];
	}

	if (mesh->HasNormals()) {
		obj->normData.resize(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < idxs.size(); i++) {
			obj->normData[i] = tmpNormals[idxs[i]];
		}
	}


	if (mesh->HasTextureCoords(0)) {
		obj->uvData.resize(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < idxs.size(); i++) {
			obj->uvData[i] = tmpUVs[idxs[i]];
		}
	}

	// load data
	Components::CObject3DHelper::setData(obj, obj->vxData, obj->uvData, obj->normData);

	return true;
}
