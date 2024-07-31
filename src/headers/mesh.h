#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2)

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern fact_t mesh_faces[N_CUBE_FACES];

typedef struct {
    vec3_t* vertices; // dynamic array of vertices
    fact_t* faces;    //   "       "   of faces
    vec3_t rotation;  // rotation with x, y, and z values
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);

#endif