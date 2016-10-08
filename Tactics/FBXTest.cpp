#ifdef TESTING


#include <gtest/gtest.h>

#include "FBX.h"

#include <fstream>
#include <regex>

using namespace std;

TEST(FBX_Test, seekg_behaves_nicely) {

	// open cube fbx
	ifstream in("assets/models/correctCube.fbx", ifstream::binary);

	// find a line with "Mesh"
	auto pos = in.tellg();
	string str1, str2;

	while (getline(in, str1)) {
		if (regex_search(str1, regex("\"Mesh\""))) {
			// reset the line
			cout << pos << endl;
			in.seekg(pos);
			getline(in, str2);
			ASSERT_EQ(str1, str2) << "The strings should be the same";
			break;
		}
		pos = in.tellg(); 
	}
}

#endif