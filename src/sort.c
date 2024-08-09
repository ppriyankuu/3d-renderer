#include <stdio.h>
#include "headers/sort.h"
#include "headers/triangle.h"

void quick_sort(triangle_t triangles_to_render[], int low, int high) {
    if (low < high) {
        int pivot_index = partition(triangles_to_render, low, high);
        quick_sort(triangles_to_render, low, pivot_index - 1);
        quick_sort(triangles_to_render, pivot_index + 1, high);
    }
}

int partition(triangle_t triangles_to_render[], int low, int high) {
    float pivot = triangles_to_render[high].avg_depth;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (triangles_to_render[j].avg_depth > pivot) {
            i++;
            triangle_t temp = triangles_to_render[i];
            triangles_to_render[i] = triangles_to_render[j];
            triangles_to_render[j] = temp;
        }
    }

    triangle_t temp = triangles_to_render[i + 1];
    triangles_to_render[i + 1] = triangles_to_render[high];
    triangles_to_render[high] = temp;

    return i + 1;
}

void sort_triangles(triangle_t triangles_to_render[], int num_of_triangles) {
    quick_sort(triangles_to_render, 0, num_of_triangles - 1);
}
