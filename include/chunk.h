#pragma once
#include <cglm/struct.h>
#include <block.h>

#define CHUNK_SIZE 4096

// store Chunk position and block ID
typedef struct {
	unsigned short blocks[16][16][16];
	ivec3s pos;
} Chunk;

// Directions
static ivec3 DIR_VECS[] = {
	{  0,  0, -1  },
	{  0,  0,  1  },
	{ -1,  0,  0  },
	{  1,  0,  0  },
	{  0, -1,  0  },
	{  0,  1,  0  }
};