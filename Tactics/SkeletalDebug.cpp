

#ifdef TESTING

#include "SkeletalAnimation.h"
#include "assimp_loader.h"
#include "BasicWorld.h"

#include "Util.h"

#include <ogldev_skinned_mesh.h>

#include <gtest/gtest.h>

#include <fstream>
#include <iostream>

using namespace Tactics;

using namespace std;


// create a basic world so we get a gl context
class SDWorld : public Worlds::BasicWorld {};


struct SDFixture : ::testing::Test {
	// create our transforms
	SDWorld world;

	Components::CObject3D obj;
	Components::SkeletalAnimation skeletal;

	SkinnedMesh skinnedMesh;
	std::vector<Matrix4f> transforms;

	SDFixture() {
		world.setup();

		LoadMesh(&obj, "assets/models/boblampclean.md5mesh");
		LoadSkeletal(&skeletal, obj.idxData, "assets/models/boblampclean.md5mesh");

		skinnedMesh.LoadMesh("assets/models/boblampclean.md5mesh");
		skinnedMesh.BoneTransform(0.f, transforms);
	}

};


string getName(map<string, unsigned int> m, unsigned int v) {
	for (auto const & kv : m) {
		if (kv.second == v) {
			return kv.first;
		}
	}
	throw exception("Could not find value");
}


ostream & operator<<(ostream & out, const glm::mat4 & mat) {
    out << std::fixed << setprecision(8);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out << mat[i][j];
			if (j < 3) out << '\t';
		}
		out << endl;
	}
	return out;
}


TEST(SkeletalAnimation, generate_debug_matrices) {
	SDWorld world;
	world.setup();

	Components::CObject3D obj;
	Components::SkeletalAnimation skeletal;

	LoadMesh(&obj, "assets/models/boblampclean.md5mesh");
	LoadSkeletal(&skeletal, obj.idxData, "assets/models/boblampclean.md5mesh");

	skeletal.BoneTransforms(0.f);

	// create debugging file
	ofstream out("debugSkeletal_tactics.dat");
	for (unsigned int i = 0; i < skeletal.bone_transforms.size(); i++) {
		// output bone name
		out << getName(skeletal.name_id, i) << endl;
		out << skeletal.bone_transforms[i] << endl;
	}
}


TEST(SkeletalDebug, ogldev_matrix_compatability) {
	Matrix4f matrix;
	matrix.InitTranslationTransform(1, 2, 3);

	glm::mat4 glmmat = glm::translate(glm::mat4(1.f), glm::vec3(1.f, 2.f, 3.f));

	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			ASSERT_FLOAT_EQ(glmmat[i][j], matrix.m[i][j]);
		}
	}
}


TEST(SkeletalDebug, ogldev_matrix_transpose) {
	Matrix4f matrix;
	matrix.InitTranslationTransform(1, 2, 3);

	glm::mat4 glmmat = glm::translate(glm::mat4(1.f), glm::vec3(1.f, 2.f, 3.f));
	glmmat = glm::transpose(glmmat);

	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			ASSERT_FLOAT_EQ(glmmat[i][j], matrix.m[i][j]);
		}
	}
}

/*
bool mateq(Matrix4f a, glm::mat4 b) {
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			if (!eqf(a.m[i][j], b[i][j])) return false;
		}
	}
	return true;
}
*/


// Test that AB=C == BtAt=Ct
TEST(SkeletalDebug, math_is_sensible) {
	glm::mat4 gmat = glm::translate(glm::mat4(1.f), glm::vec3(1.f, 2.f, 3.f)) * glm::translate(glm::mat4(1.f), glm::vec3(2.f, 3.f, 4.f));

	Matrix4f ma;
	ma.InitTranslationTransform(2.f, 3.f, 4.f);
	Matrix4f mb;
	mb.InitTranslationTransform(1.f, 2.f, 3.f);
	auto mmat = ma * mb;

	ASSERT_TRUE(mateq(mmat, glm::transpose(gmat)));
}


// make sure the bone space matrices are equivalent after loading
TEST_F(SDFixture, bone_space_matrices) {
	for (unsigned int i = 0; i < skeletal.bones.size(); i++) {
		ASSERT_TRUE(mateq(skinnedMesh.m_BoneInfo[i].BoneOffset, skeletal.bones[i].boneSpace)) << 
			"Matrices differ at index " << i << endl <<
			"Matrix4f" << endl << skinnedMesh.m_BoneInfo[i].BoneOffset << endl << endl << 
			"mat4" << endl << skeletal.bones[i].boneSpace << endl;
	}
}


// make sure the same bones are mapped to the same indices
TEST_F(SDFixture, bone_mapping) {
	std::vector<string> a;
	for (auto pr : skeletal.name_id) {
		a.push_back(pr.first);
	}

	std::vector<string> b;
	for (auto pr : skinnedMesh.m_BoneMapping) {
		b.push_back(pr.first);
	}

	// assert that the list of names for each implementation is equal
	ASSERT_EQ(a, b);

	// assert that the maps are equivalent
	ASSERT_EQ(skeletal.name_id, skinnedMesh.m_BoneMapping);
}


TEST_F(SDFixture, bone_space_debug) {
	for (int i = 0; i < skeletal.bones.size(); i++) {
		ASSERT_TRUE(mateq(skeletal.debug_bones[i], skeletal.bones[i].boneSpace));
	}
}


TEST_F(SDFixture, debug_transforms_match) {
	skeletal.debug_transforms.resize(skeletal.bones.size());
	Matrix4f id;
	id.InitIdentity();
	vector<glm::mat4> trace;
	skeletal.ReadNodeHeirarchyDebug(0, skeletal.pscene->mRootNode, id, trace);

	vector<Matrix4f> mtrace;
	skinnedMesh.ReadNodeHeirarchyDebug(0, skinnedMesh.m_pScene->mRootNode, id, mtrace);

	for (int i = 0; i < skeletal.bone_transforms.size(); i++) {
		for (unsigned int r = 0; r < 4; r++) {
			for (unsigned int c = 0; c < 4; c++) {
				ASSERT_FLOAT_EQ(skeletal.debug_transforms[i].m[r][c], skinnedMesh.m_BoneInfo[i].FinalTransformation.m[r][c]);
			}
		}
	}
}


TEST_F(SDFixture, transforms_debug) {
	skeletal.debug_transforms.resize(skeletal.bones.size());
	Matrix4f id;
	id.InitIdentity();
	vector<glm::mat4> trace;
	skeletal.ReadNodeHeirarchyDebug(0, skeletal.pscene->mRootNode, id, trace);
	for (int i = 0; i < skeletal.bone_transforms.size(); i++) {
		ASSERT_TRUE(mateq(skeletal.debug_transforms[i], skeletal.bone_transforms[i]));
	}
}

/*
TEST_F(SDFixture, trace_length) {
	std::vector<glm::mat4> a;
	skeletal.ReadNodeHeirarchyDebug(0, skeletal.pscene->mRootNode, glm::mat4(1.f), a);

	std::vector<Matrix4f> b;
	Matrix4f id;
	id.InitIdentity();
	skinnedMesh.ReadNodeHeirarchyDebug(0, skinnedMesh.m_pScene->mRootNode, id, b);

	ASSERT_EQ(a.size(), b.size());
}
*/

/*
TEST_F(SDFixture, trace_equiv) {
	std::vector<glm::mat4> a;
	skeletal.ReadNodeHeirarchyDebug(0, skeletal.pscene->mRootNode, glm::mat4(1.f), a);

	std::vector<Matrix4f> b;
	Matrix4f id;
	id.InitIdentity();
	skinnedMesh.ReadNodeHeirarchyDebug(0, skinnedMesh.m_pScene->mRootNode, id, b);

	for (unsigned int i = 0; i < a.size(); i++) {
		ASSERT_TRUE(mateq(b[i],a[i])) << 
			"Matrices differ at index " << i << endl <<
			"Matrix4f" << endl << a[i] << endl << endl <<
			"mat4" << endl << b[i] << endl;
	}
}
*/

TEST(SkeletalAnimation, check_time0) {
	// create our transforms
	SDWorld world;
	world.setup();

	Components::CObject3D obj;
	Components::SkeletalAnimation skeletal;

	LoadMesh(&obj, "assets/models/boblampclean.md5mesh");
	LoadSkeletal(&skeletal, obj.idxData, "assets/models/boblampclean.md5mesh");

	skeletal.BoneTransforms(0.f); 

	// ogldev transforms
	SkinnedMesh skinnedMesh;
	skinnedMesh.LoadMesh("assets/models/boblampclean.md5mesh");
	std::vector<Matrix4f> transforms;
	skinnedMesh.BoneTransform(0.f, transforms);

	for (unsigned int i = 0; i < skeletal.bone_transforms.size(); i++) {
		ASSERT_TRUE(mateq(transforms[i], skeletal.bone_transforms[i])) << "Matrices differ at index " << i << endl <<
			"Matrix4f" << endl << transforms[i] << endl << endl << "mat4" << endl << skeletal.bone_transforms[i] << endl;
	}

} // check_time0


#endif
