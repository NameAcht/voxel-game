#pragma once
#include <cglm/struct.h>
#include <block.h>

#define CHUNK_SIZE 16
#define CHUNK_FACE CHUNK_SIZE*CHUNK_SIZE
#define CHUNK_VOL CHUNK_FACE*CHUNK_SIZE

// store Chunk position and block ID
// indexing block data:
// to move +-1 on X axis -> add +-1 to index
// to move +-1 on Y axis -> add +-CHUNK_SIZE to index
// to move +-1 on Z axis -> add +-CHUNK_FACE to index
typedef struct {
	unsigned short blocks[CHUNK_VOL];
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