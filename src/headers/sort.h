#ifndef SORT_H
#define SORT_H

#include "triangle.h"

void quick_sort(triangle_t triangles_to_render[], int low, int high);
int partition(triangle_t triangles_to_render[], int low, int high);
void sort_triangles(triangle_t triangles_to_render[], int num_of_triangles);

#endif