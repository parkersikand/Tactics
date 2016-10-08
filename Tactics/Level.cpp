
#include "BasicDrawSystem.h"
#include "Level.h"
#include "Util.h"
#include "OBJLoader.h"

using namespace Tactics;

Level::Level(int _height, int _width, int * _heightMap, GroundType * _groundMap) : width(_width), height(_height) {
	// load data
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			heightMap[i][j] = _heightMap[i*width + j];
			groundMap[i][j] = _groundMap[i*width + j];
		}
	}
}

void Level::load(ECS::World & world) {
	
	std::map<GroundType, GLuint> type2tex;

	// load textures
	// throwaway component
	Components::CObject3D o3d;

	// grass
	glGenTextures(1, &o3d.texId);
	Components::CObject3DHelper::loadTexture2Dpng(&o3d, "assets/models/grass1.png");
	type2tex[Grass] = o3d.texId;

	// stone
	glGenTextures(1, &o3d.texId);
	Components::CObject3DHelper::loadTexture2Dpng(&o3d, "assets/models/stone1.png");
	type2tex[Stone] = o3d.texId;
	
	// cube data
	std::vector<glm::vec3> vx, norm;
	std::vector<glm::vec2> uv;
	LoadObj("assets/models/correctCube.obj", vx, uv, norm);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			for (int curHeight = 0; curHeight < heightMap[i][j]; curHeight++) {
				ECS::EntityHdl cube = world.newEntity();
				world.addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem> >(cube);
				auto * cubePos = world.addComponent<Components::Position3D<> >(cube);
				cubePos->x = 2.f * i;
				cubePos->z = 2.f * j;
				cubePos->y = 2.f * curHeight;
				auto * cube3d = world.addComponent<Components::CObject3D>(cube);
				//make_cube(cube3d);
				Components::CObject3DHelper::setData(cube3d, vx, uv, norm);

				cube3d->texId = type2tex[groundMap[i][j]];

			} // for
		}// for
	}// for

} // Level::load
