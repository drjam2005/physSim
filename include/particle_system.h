#pragma once
#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <raylib.h>

enum PARTICLE_TYPE {
	NONE,
	STATIC,
	SOLID,
	FLUID,
	GAS,
};


//std::unordered_map<PARTICLE_TYPE, std::pair<std::pair<Color*, Texture*>, Color>> bufferMaps;
struct ParticleInfo {
	Color* buffer;
	Texture* tex;
	Color clr;
};


#endif
