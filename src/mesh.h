#ifndef __MESH__
#define __MESH__
#include "base.h"

typedef struct Vertex3D {
    f32 x, y, z;
    f32 r, g, b, a;
} Vertex3D;

static Vertex3D vertices[] = {
    {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f},
    {-0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
    {0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f},
    {0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
};

static u32 indices[] = {0, 1, 2, 0, 2, 3};

#endif /* __MESH__ */