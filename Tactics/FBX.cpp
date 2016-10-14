

#include "FBX.h"
#include "Object3D.h"
#include "SkeletalAnimation.h"
#include "Util.h"

#include <fstream>
#include <map>
#include <regex>

using namespace std;

// stream should be at start of line containing "Properties60:"
map<string, pair<string, string> > readProperties60(istream & in) {
	map<string, pair<string, string> > out;
	string buf;
	
	in >> buf;
	assert(buf == "Properties60:");

	// discard '{'
	in >> buf;
	string key, type, value;
	for (int i = 0; i < 71; i++) {
		in >> buf;
		assert(buf == "Property:");
//		in >> key;
//		key = key.substr(1, key.length() - 3);
		quotedString(in, key);
		in.get();
		//in >> type;
		quotedString(in, type);
		in.get();
		getline(in, value);
		out[key] = make_pair(type, value);
	}
	// discard '}'
	in >> buf;

	return out;
}

// stream should be at start of line containing Model node header
void readMeshData(istream & in, vector<glm::vec3> & vx, vector<glm::vec2> & uv, vector<glm::vec3> & norm, vector<unsigned int> & idx) {
	string buf;
	auto currentPosition = in.tellg();
	vector<glm::vec3> inVertices;
	vector<int> vertexIndices, inUVidx;
	vector<float> inNormalData, inUVData;

	// discard the header line
	getline(in, buf);

	while (buf != "}") {
		// mark current position
		currentPosition = in.tellg();
		in >> buf;
		
		if (buf == "Properties60:") {
			// move back
			in.seekg(currentPosition);
			auto props = readProperties60(in);
		}
		else if (buf == "Vertices:") {
			float num1, num2, num3;
			while (true) {
				in >> num1;
				in.get();
				in >> num2;
				in.get();
				in >> num3;
				inVertices.push_back(glm::vec3(num1, num2, num3));
				if (in.peek() != ',') break;
				in.get();
			}
		}
		else if (buf == "PolygonVertexIndex:") {
			Tactics::Util::FBX::readMany<int>(in, vertexIndices);
		}
		else if (buf == "LayerElementNormal:") {
			// discard rest of line
			getline(in, buf);
			// find "Normals:"
			while (in >> buf && buf != "Normals:");
			Tactics::Util::FBX::readMany<float>(in, inNormalData);
			while (in >> buf && buf != "}"); // escape block
			buf = "";
		}
		else if (buf == "LayerElementUV:") {
			getline(in, buf);
			// find "UV:"
			while (in >> buf && buf != "UV:");
			Tactics::Util::FBX::readMany<float>(in, inUVData);
			while (in >> buf && buf != "UVIndex:");
			Tactics::Util::FBX::readMany<int>(in, inUVidx);
			while (in >> buf && buf != "}"); // escape block
			buf = "";
		}

	} // while

	// assemble vertex info, triangulate faces
	std::vector<unsigned int> order;
	vx.clear();
	unsigned int i = 0;
	while (i < vertexIndices.size()) {
		if (i + 2 < vertexIndices.size() && vertexIndices[i + 2] < 0) { // triangle
			vx.push_back(inVertices[vertexIndices[i]]);
			vx.push_back(inVertices[vertexIndices[i + 1]]);
			vx.push_back(inVertices[-1 - vertexIndices[i + 2]]);
			idx.push_back(vertexIndices[i]);
			idx.push_back(vertexIndices[i + 1]);
			idx.push_back(-1 - vertexIndices[i + 2]);
			order.push_back(i);
			order.push_back(i+1);
			order.push_back(i+2);
			i += 3;
		}
		else if (i + 3 < vertexIndices.size() && vertexIndices[i + 3] < 0) { // quad
			vx.push_back(inVertices[vertexIndices[i]]);
			vx.push_back(inVertices[vertexIndices[i + 1]]);
			vx.push_back(inVertices[vertexIndices[i + 2]]);
			vx.push_back(inVertices[vertexIndices[i]]);
			vx.push_back(inVertices[vertexIndices[i + 2]]);
			vx.push_back(inVertices[-1 - vertexIndices[i + 3]]);
			idx.push_back(vertexIndices[i]);
			idx.push_back(vertexIndices[i + 1]);
			idx.push_back(vertexIndices[i + 2]);
			idx.push_back(vertexIndices[i]);
			idx.push_back(vertexIndices[i + 2]);
			idx.push_back(-1 - vertexIndices[i + 3]);
			order.push_back(i);
			order.push_back(i + 1);
			order.push_back(i + 2);
			order.push_back(i);
			order.push_back(i + 2);
			order.push_back(i + 3);
			i += 4;
		}
		else { // n-gon
			int start = i;
			while (i + 2 > 0) {
				vx.push_back(inVertices[vertexIndices[start]]);
				vx.push_back(inVertices[vertexIndices[i + 1]]);
				vx.push_back(inVertices[vertexIndices[i + 2]]);
				idx.push_back(vertexIndices[start]);
				idx.push_back(vertexIndices[i + 1]);
				idx.push_back(-1 - vertexIndices[i + 2]);
				order.push_back(start);
				order.push_back(i + 1);
				order.push_back(i + 2);
				i++;
			}
			vx.push_back(inVertices[vertexIndices[start]]);
			vx.push_back(inVertices[vertexIndices[i + 1]]);
			vx.push_back(inVertices[-1 - vertexIndices[i + 2]]);
			idx.push_back(vertexIndices[start]);
			idx.push_back(vertexIndices[i + 1]);
			idx.push_back(-1 - vertexIndices[i + 2]);
			order.push_back(start);
			order.push_back(i + 1);
			order.push_back(i + 2);
		} // n-gon

	} // while

	// normal info
	norm.clear();
	norm.resize(vx.size());
	std::vector<glm::vec3> tmpNorm;
	for (unsigned int i = 0, n = 0; i < inNormalData.size(); i += 3, n++) {
	//	norm[n] = glm::vec3(inNormalData[i], inNormalData[i+1], inNormalData[i+2]);
		tmpNorm.push_back(glm::vec3(inNormalData[i], inNormalData[i + 1], inNormalData[i + 2]));
	}
	for (unsigned int i = 0; i < order.size(); i++) {
		norm[i] = tmpNorm[order[i]];
	}

	// uv info
	// convert list of floats into vector of points, ordered by vertex id
	vector<glm::vec2> tmpUV;
	tmpUV.resize(inUVidx.size());
	for (unsigned int i = 0; i < inUVData.size(); i += 2) {
		tmpUV[i / 2] = glm::vec2(inUVData[i], inUVData[i + 1]);
	}
 
	uv.clear();
	uv.resize(idx.size());
	for (unsigned int i = 0; i < idx.size(); i++) {
		uv[i] = tmpUV[inUVidx[order[i]]];
	}

	std::cout << "Read " << vx.size() << " vertices" << std::endl;
	std::cout << "Read " << uv.size() << " uvs" << std::endl;
	std::cout << "Read " << norm.size() << " normals" << std::endl;

} // readMeshData


bool Tactics::Util::FBX::LoadMesh(const char * filename, Tactics::Components::CObject3D * o3d, bool flipUVs, const char * meshname) {
	string line;
	ifstream in(filename, ifstream::binary); // use binary mode so it preserves all bytes
	auto currentPosition = in.tellg();

	cout << "Reading \"" << filename << "\"" << endl;

	// find "Mesh" header
	while (getline(in, line)) {
		if (regex_search(line, regex("Model:.*\"Mesh\"") )) {
			// extract the mesh name
			string meshnameFound;
			smatch matchObject;
			regex_match(line, matchObject, regex(".*Model::(\\w+).*"));
			assert(matchObject.size() == 2); // full match + name
			meshnameFound = matchObject[1];
			if (!meshname || meshname == "" || meshname == meshnameFound) {
				// back up to beginning of line
				in.seekg(currentPosition);
				std::vector<glm::vec3> vx, norm;
				std::vector<glm::vec2> uv;
				std::vector<unsigned int> idx;
				readMeshData(in, vx, uv, norm, idx);
				if (flipUVs) {
					for (auto & x : uv) {
						x.y = 1.f - x.y;
					}
				}
				Components::CObject3DHelper::setData(o3d, vx, uv, norm);
				o3d->idxData = idx;
				return true;
			}
		}
		currentPosition = in.tellg();
	}

	return false;
} // LoadMesh


glm::mat4 vec2mat(vector<float> & v) {
	glm::mat4 out;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out[i][j] = v[4 * i + j];
		}
	}
	return out;
}


// parse Deformer node
void ParseDeformerNode(istream & in, string & meshName, string & boneName, map<int, float> & index_weights, glm::mat4 & transform, glm::mat4 & transformLink) {
	smatch names;
	string line;
	getline(in, line);
	getline(in, line);
	regex_match(line, names, regex("\\W+Deformer: \"SubDeformer::Cluster (\\w+) (\\w+)\".*"));
	assert(names.size() == 3);
	meshName = names[1];
	boneName = names[2];
	// find "Indexes:"
	while (in >> line && line != "Indexes:");
	vector<int> idx;
	Tactics::Util::FBX::readMany<int>(in, idx);
	// find "Weights:"
	while (in >> line && line != "Weights:");
	vector<float> weights;
	Tactics::Util::FBX::readMany<float>(in, weights);
	assert(idx.size() == weights.size());
	// build map
	for (unsigned int i = 0; i < idx.size(); i++) {
		index_weights[idx[i]] = weights[i];
	}
	while (in >> line && line != "Transform:");
	vector<float> vf;
	Tactics::Util::FBX::readMany<float>(in, vf);
	transform = vec2mat(vf);
	while (in >> line && line != "TransformLink:");
	Tactics::Util::FBX::readMany<float>(in, vf);
	transformLink = vec2mat(vf);
	// consume "}"
	while (in >> line && line != "}");
}


map<string, vector<string>> ParseConnectionsNode(istream & in) {
	string discard, a, b;
	string aname, bname;
	map<string, vector<string>> adj;

	// discard header
	getline(in, discard);
	getline(in, discard);
	getline(in, discard);
	while (in >> discard && discard == "Connect:") {
		in >> discard;
		quotedString(in, a);
		quotedString(in, b);
		//aname = a.substr(8, a.size() - 9);
		//bname = b.substr(8, b.size() - 9);
		adj[b].push_back(a);
	}
	
	return adj;
} // ParseConnectionsNode

/*
Tactics::SkeletalAnimation::Bone LoadBoneHierarchy(const char * filename, const char * meshname) {

}
*/

void ParseBoneKeyframes(istream & in, vector<glm::vec3> & translations, vector<glm::vec3> & rotations, vector<glm::vec3> & scales) {
	string buf;
    // find keycount
	int keyCount;
	while (in >> buf && buf != "KeyCount:");
	in >> keyCount;

	translations.resize(keyCount);
	rotations.resize(keyCount);
	scales.resize(keyCount);

	vector<glm::vec3> * varr[3] = { &translations, &rotations, &scales };

	for (int i = 0; i < 3; i++) {
		auto * v = varr[i];
		for (int j = 0; j < 3; j++) {
			// find "Key:"
			while (in >> buf && buf != "Key:");
			for (int k = 0; k < keyCount; k++) {
				in >> buf;
				smatch valMatch;
				regex_match(buf, valMatch, regex("\\d+,(-?\\d+.\\d+),.*"));
				assert(valMatch.size() == 2);
				(*v)[k][j] = stof(valMatch[1]);
			}
		}
	}
} // ParseBoneKeyFrames


typedef Tactics::Components::SkeletalAnimation::Animation::KeyFrame KF;
typedef Tactics::SkeletalAnimation::Bone Bone;

void ParseKeyframes(istream & in, vector<KF> & keyframes) {
	string line;
	smatch boneNameMatch;

	struct Transforms {
		std::vector<glm::vec3> t;
		std::vector<glm::vec3> r;
		std::vector<glm::vec3> s;
	};

	map<string, Transforms> boneTransforms;

	while (in >> line) {
		if (regex_match(line, boneNameMatch, regex(".*Model::(\\w+).*"))) {
			auto & t = boneTransforms[boneNameMatch[1]];
			ParseBoneKeyframes(in, t.t, t.r, t.s);
		}
	} // end of file

	// aggregate data
	keyframes.resize(boneTransforms.begin()->second.r.size());
	for (unsigned int kf = 0; kf < keyframes.size(); kf++) {
		keyframes[kf].frame = kf;
		for (auto const & name_trans : boneTransforms) {
			auto r = name_trans.second.r[kf];
			keyframes[kf].bones[name_trans.first].rotation = glm::rotate(glm::mat4(), glm::radians(r.x), glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(), glm::radians(r.y), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(), glm::radians(r.z), glm::vec3(0, 0, 1));
			keyframes[kf].bones[name_trans.first].scale = glm::scale(glm::mat4(), name_trans.second.s[kf]);
			keyframes[kf].bones[name_trans.first].translation = glm::translate(glm::mat4(), name_trans.second.t[kf]);
		}
	}

} // ParseKeyFrames

void ParseBoneNode(istream & in, Bone & bone) {
	string buf;
	auto pos = in.tellg();
	while (in >> buf && buf != "Properties60:") pos = in.tellg();
	in.seekg(pos);
	auto p60 = readProperties60(in);
	
	smatch vecMatch;
	// translation
	auto str = p60["Lcl Translation"].second;
	regex_match(str, vecMatch, regex(".*(-?\\d+.\\d+),(-?\\d+.\\d+),(-?\\d+.\\d+)"));
	assert(vecMatch.size() == 4);
	bone.translation = glm::translate(glm::mat4(), glm::vec3(stof(vecMatch[1]), stof(vecMatch[2]), stof(vecMatch[3])));
	// scale
	regex_match(p60["Lcl Scaling"].second, vecMatch, regex(".*(-?\\d+.\\d+),(-?\\d+.\\d+),(-?\\d+.\\d+)"));
	assert(vecMatch.size() == 4);
	bone.scale = glm::scale(glm::mat4(), glm::vec3(stof(vecMatch[1]), stof(vecMatch[2]), stof(vecMatch[3])));
	// rotation
	regex_match(p60["Lcl Rotation"].second, vecMatch, regex(".*(-?\\d+.\\d+),(-?\\d+.\\d+),(-?\\d+.\\d+)"));
	assert(vecMatch.size() == 4);
	bone.rotation =
		glm::rotate(glm::mat4(), glm::radians(stof(vecMatch[1])), glm::vec3(1, 0, 0))
		* glm::rotate(glm::mat4(), glm::radians(stof(vecMatch[2])), glm::vec3(0, 1, 0))
		* glm::rotate(glm::mat4(), glm::radians(stof(vecMatch[3])), glm::vec3(0, 0, 1));
	while (in >> buf && buf != "}");
}

bool Tactics::Util::FBX::LoadSkeletalAnimation(const char * filename, Tactics::Components::SkeletalAnimation * sa, Tactics::Components::CObject3D * o3d, const char * meshname) {

	ifstream in(filename, istream::binary);
	cout << "Reading \"" << filename << "\"" << endl;
	auto pos = in.tellg();

	map<string, map<int, float>> bone_weights;
	map<string, vector<string>> bone_hierarchy;
	vector<KF> keyframes;

	// map of bone names to bone objects
	map<string, Bone> name_bones;

	string buf;
	while (in >> buf) {
		if (buf == "Deformer:") {
			in >> buf;
			if (buf.substr(1, 11) == "SubDeformer") {
				in.seekg(pos);
				string meshName, boneName;
				map<int, float> index_weights;
				glm::mat4 transform, transformLink;
				ParseDeformerNode(in, meshName, boneName, index_weights, transform, transformLink);
				bone_weights[boneName] = index_weights;
				//name_bones[boneName].boneSpace = transform; // down, CCW
				name_bones[boneName].boneSpace = transformLink; // down, CCW
			}
		}
		else if (buf == "Connections:") {
			in.seekg(pos);
			bone_hierarchy = ParseConnectionsNode(in);
		}
		else if (buf == "Takes:") {
			in.seekg(pos);
			ParseKeyframes(in, keyframes);
		}
		else if (buf == "Relations:") {
			// skip over relations section
			while (in >> buf && buf != ";");
		}
		else if (buf == "Model:") {
			// check if this is a limb
			std::string name, check;
			quotedString(in, name);
			in.get(); // discard ','
			quotedString(in, check);
			if (check == "Limb") {
				// extract bone name
				smatch boneNameMatch;
				regex_match(name, boneNameMatch, regex("Model::(\\w+)"));
				assert(boneNameMatch.size() == 2);
				in.seekg(pos);
//				name_bones[boneNameMatch[1]] = ParseBoneNode(in);
				ParseBoneNode(in, name_bones[boneNameMatch[1]]);
			}
		}
		pos = in.tellg();
	} // while in >> buf

	sa->bones.resize(bone_hierarchy.size());
	
	// assign ids to bones
	map<string, unsigned int> bone_idxs;
	int i = 0;
	for (auto const & p : bone_weights) {
		bone_idxs[p.first] = i;
		sa->bones[i] = name_bones[p.first];
		sa->bones[i].boneName = p.first;
		i++;
	}
	sa->name_id = bone_idxs;

	// map of vertex ids to bone info
	map<unsigned int, Components::SkeletalAnimation::VertexBoneInfo> idx_boneInfo;
	for (auto const & p : bone_weights) {
		for (auto const & vi_weight : p.second) {
			auto & bi = idx_boneInfo[vi_weight.first];
			// find first available slot
			int slot = 0;
			while (bi.boneWeight[slot] > 0) slot++;
			if (slot < MAX_BONES) {
				bi.boneId[slot] = bone_idxs[p.first];
				bi.boneWeight[slot] = vi_weight.second;
			}
		}
	}

	// copy vertex bone info
	sa->vertexBoneInfo.resize(o3d->count);
	for (i = 0; i < o3d->count; i++) {
		sa->vertexBoneInfo[i] = idx_boneInfo[o3d->idxData[i]];
	}

	// create default animation
	auto & anim = sa->animations["default"];
	i = 1;
	for (auto & kf : keyframes) {
		kf.animation = &anim;
		anim.keyFrames[i * 1.f] = kf;
		i++;
	}

	sa->hierarchy = bone_hierarchy;

	// set armature rotation
	sa->armatureRotation = name_bones["Armature"].rotation;

	// copy data to GPU
	glGenBuffers(1, &sa->vertexBoneInfoVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sa->vertexBoneInfoVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Components::SkeletalAnimation::VertexBoneInfo) * sa->vertexBoneInfo.size(), &sa->vertexBoneInfo[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
} // LoadSkeletalAnimation
