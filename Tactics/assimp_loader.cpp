
#include "assimp_loader.h"

#include <magick++.h>

#include <iostream>
#include <stack>

using namespace std;
using namespace Tactics;

// Load all meshes into one object
bool LoadMesh(Tactics::Components::CObject3D * obj, const char * filename) {

	Assimp::Importer importer;

	const aiScene * scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals);

	if (!scene) {
		cout << importer.GetErrorString() << endl;
		return false;
	}

	obj->vxData.clear();
	obj->uvData.clear();
	obj->normData.clear();

	for (unsigned int m = 0; m < scene->mNumMeshes; m++) {

		auto * mesh = scene->mMeshes[m];

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
		unsigned int offset = obj->idxData.size();
		for (unsigned int i = 0, f = 0; f < mesh->mNumFaces; f++, i += 3) {
			idxs[i] = offset + mesh->mFaces[f].mIndices[0];
			idxs[i + 1] = offset + mesh->mFaces[f].mIndices[1];
			idxs[i + 2] = offset + mesh->mFaces[f].mIndices[2];
		}
		//obj->idxData = idxs;
		obj->idxData.insert(obj->idxData.end(), idxs.begin(), idxs.end());

		obj->vxData.resize(obj->vxData.size() + mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < idxs.size(); i++) {
			obj->vxData[i+offset] = tmpVertices[idxs[i]-offset];
		}

		if (mesh->HasNormals()) {
			obj->normData.resize(obj->normData.size() + mesh->mNumFaces * 3);
			for (unsigned int i = 0; i < idxs.size(); i++) {
				obj->normData[i+offset] = tmpNormals[idxs[i]-offset];
			}
		}

		if (mesh->HasTextureCoords(0)) {
			obj->uvData.resize(obj->uvData.size() + mesh->mNumFaces * 3);
			for (unsigned int i = 0; i < idxs.size(); i++) {
				obj->uvData[i+offset] = tmpUVs[idxs[i]-offset];
			}
		}

	} // for each mesh

	// load data
	Components::CObject3DHelper::setData(obj, obj->vxData, obj->uvData, obj->normData);

	return true;
}


glm::mat4 aiMatrix4x4_mat4(aiMatrix4x4 input) {
	glm::mat4 output;
	for (unsigned int i = 0; i < 4; i++)
		for (unsigned int j = 0; j < 4; j++)
			output[i][j] = input[i][j];
	return output;
}


glm::mat4 aiMatrix3x3_mat4(aiMatrix3x3 input) {
	glm::mat4 output(1.f); // initialize identity matrix
	for (unsigned int i = 0; i < 3; i++)
		for (unsigned int j = 0; j < 3; j++)
			output[i][j] = input[i][j];
	return output;
}


bool LoadSkeletal(Components::SkeletalAnimation * skeletal, vector<unsigned int> & idx, const char * filename) {

	skeletal->pscene = skeletal->importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals);

	if (!skeletal->pscene) {
		std::cout << skeletal->importer.GetErrorString() << std::endl;
		return false;
	}

	auto globalInverseTransform = skeletal->pscene->mRootNode->mTransformation;
	globalInverseTransform.Inverse();
	skeletal->globalInverseTransform = aiMatrix4x4_mat4(globalInverseTransform);
	skeletal->globalInverseTransformM4 = Matrix4f(globalInverseTransform);

	map<string, unsigned int> boneNameIdMap;
	unsigned int boneIdxCounter = 0;

	// TODO need way to go from indexed data to raw data
	map<unsigned int, map<unsigned int, float>> vertexId_boneId_weight;

	for (unsigned int m = 0; m < skeletal->pscene->mNumMeshes; m++) {

		auto offset = 0;
		for (unsigned int mprev = 0; mprev < m; mprev++) {
			offset += skeletal->pscene->mMeshes[mprev]->mNumFaces * 3;
		}

		auto * pmesh = skeletal->pscene->mMeshes[m];

		for (unsigned int i = 0; i < pmesh->mNumBones; i++) {
			unsigned int BoneIndex;
			string BoneName = pmesh->mBones[i]->mName.data;

			if (boneNameIdMap.find(BoneName) == boneNameIdMap.end()) {
				BoneIndex = boneIdxCounter;
				boneIdxCounter++;
				SkeletalAnimation::Bone bone;
				skeletal->bones.push_back(bone);
				boneNameIdMap[BoneName] = BoneIndex;
			}
			else {
				BoneIndex = boneNameIdMap[BoneName];
			}

			skeletal->bones[BoneIndex].boneSpace = aiMatrix4x4_mat4(pmesh->mBones[i]->mOffsetMatrix);
			skeletal->debug_bones.resize(skeletal->bones.size());
			skeletal->debug_bones[BoneIndex] = Matrix4f(pmesh->mBones[i]->mOffsetMatrix);

			for (unsigned int j = 0; j < pmesh->mBones[i]->mNumWeights; j++) {
				vertexId_boneId_weight[pmesh->mBones[i]->mWeights[j].mVertexId+offset][BoneIndex] =
					pmesh->mBones[i]->mWeights[j].mWeight;
			}

		} // for each bone
	} // for each mesh

	skeletal->name_id = boneNameIdMap;
	skeletal->bone_transforms.resize(skeletal->bones.size());

	// create vector of vertex bone info
	skeletal->vertexBoneInfo.resize(idx.size());
	for (unsigned int i = 0; i < idx.size(); i++) {
		auto boneId_weight = vertexId_boneId_weight[idx[i]];
		if (boneId_weight.size() > 4) {
			// too big, error
			std::cout << "Too many bone influences" << std::endl;
			throw exception("Too many bone influences");
		}
		else {
			unsigned int bix = 0;
			for (auto const & bid_w : boneId_weight) {
				skeletal->vertexBoneInfo[i].boneId[bix] = bid_w.first;
				skeletal->vertexBoneInfo[i].boneWeight[bix] = bid_w.second;
				bix++;
			}
		}
	}

	// create VBO for vertex bone info
	glGenBuffers(1, &skeletal->vertexBoneInfoVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skeletal->vertexBoneInfoVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skeletal->vertexBoneInfo[0]) * skeletal->vertexBoneInfo.size(), &skeletal->vertexBoneInfo[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// save default animation
	skeletal->defaultAnimation = skeletal->pscene->mAnimations[0];

	// store all node animations
	std::stack<aiNode *> nstack;
	nstack.push(skeletal->pscene->mRootNode);
	while (nstack.size()) {
		aiNode * node = nstack.top();
		nstack.pop();

		for (unsigned int i = 0; i < skeletal->defaultAnimation->mNumChannels; i++) {
			aiNodeAnim * pNodeAnim = skeletal->defaultAnimation->mChannels[i];

			if (std::string(pNodeAnim->mNodeName.data) == std::string(node->mName.data)) {
				skeletal->named_animations[string(node->mName.C_Str())] = pNodeAnim;
			}
		}

		for (unsigned int c = 0; c < node->mNumChildren; c++) {
			nstack.push(node->mChildren[c]);
		}
	}

	return true;
}


bool LoadMultiMesh(Tactics::Components::MultiObject3D * multi, const char * filename) {
	Assimp::Importer importer;

	const aiScene * scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals);

	if (!scene) {
		cout << importer.GetErrorString() << endl;
		return false;
	}

	string dir;

	auto LastSlash = string(filename).find_last_of('/');
	if (LastSlash == string::npos) {
		dir = ".";
	}
	else if (LastSlash == 0) {
		dir = "/";
	}
	else {
		dir = string(filename).substr(0,LastSlash);
	}

	// Load textures
	std::vector<GLuint> textures;
	for (unsigned int t = 0; t < scene->mNumMaterials; t++) {
		const aiMaterial* pMaterial = scene->mMaterials[t];
		if (pMaterial->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE) > 0) {
			aiString path;
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				
				string p(path.data);

				if (p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}

				string FullPath = dir + "/" + p;
				
				Magick::Blob blob;
				Magick::Image img;
				try {
					img = Magick::Image(FullPath);
					img.write(&blob, "RGBA");
				}
				catch (Magick::Error & err) {
					std::cout << "Error loading texture '" << FullPath << "': " << err.what() << std::endl;
					continue;
				}

				GLuint texId;
				glGenTextures(1, &texId);
				glBindTexture(GL_TEXTURE_2D, texId);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.columns(), img.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				textures.push_back(texId);
			}
		}
	} // for each texture



	multi->objects.resize(scene->mNumMeshes);
	multi->offsets.resize(scene->mNumMeshes);
	for (unsigned int m = 0; m < scene->mNumMeshes; m++) {

		unsigned int offset = 0;
		for (unsigned int o = 0; o < m; o++) {
			offset += multi->objects[o].count;
		}
		multi->offsets[m] = offset;

		auto & obj = multi->objects[m];
		obj.vxData.clear();
		obj.uvData.clear();
		obj.normData.clear();

		auto * mesh = scene->mMeshes[m];

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
		//unsigned int offset = obj.idxData.size();
		for (unsigned int i = 0, f = 0; f < mesh->mNumFaces; f++, i += 3) {
			idxs[i] = mesh->mFaces[f].mIndices[0];
			idxs[i + 1] = mesh->mFaces[f].mIndices[1];
			idxs[i + 2] = mesh->mFaces[f].mIndices[2];
		}
		

		obj.vxData.resize(obj.vxData.size() + mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < idxs.size(); i++) {
			obj.vxData[i] = tmpVertices[idxs[i]];
		}

		if (mesh->HasNormals()) {
			obj.normData.resize(obj.normData.size() + mesh->mNumFaces * 3);
			for (unsigned int i = 0; i < idxs.size(); i++) {
				obj.normData[i] = tmpNormals[idxs[i]];
			}
		}

		if (mesh->HasTextureCoords(0)) {
			obj.uvData.resize(obj.uvData.size() + mesh->mNumFaces * 3);
			for (unsigned int i = 0; i < idxs.size(); i++) {
				obj.uvData[i] = tmpUVs[idxs[i]];
			}
		}

		// add offset to indices
		for (unsigned int i = 0; i < idxs.size(); i++) {
			idxs[i] += offset;
		}
		obj.idxData.insert(obj.idxData.end(), idxs.begin(), idxs.end());

		// load data
		Components::CObject3DHelper::setData(&obj, obj.vxData, obj.uvData, obj.normData);

		if (textures.size() >= mesh->mMaterialIndex + 1)
			obj.texId = textures[mesh->mMaterialIndex];

	} // for each mesh		 

	return true;
}

