#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "../primitives.h"

using namespace std;

bool loadOBJ(
    const char * path,
    vector<vec3> & out_vertices,
    vector<vec2> & out_uvs,
    vector<vec3> & out_normals
);


#endif
