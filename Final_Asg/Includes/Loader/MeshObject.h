#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <GL/gl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "../primitives.h"

using namespace std;

class MeshObject
{
  private:
    vector<vec3> vertices;
    vector<vec2> uvs;
    vector<vec3> normals;
    string texture_material;
    GLuint texture;

  public:
    MeshObject();
    bool load_obj(const char * path);
    void bind_texture(GLuint texture);
    string get_texture_mtl();
    void draw();
};


#endif
