#ifndef COLOURS_H
#define COLOURS_H

#include <GL/gl.h>
#include <stdlib.h>
#include <unordered_map>
using namespace std;

GLfloat* make_color(float r, float g, float b);

unordered_map<string, GLfloat*> create_color_table();

const char** available_colors();

int num_colors();

#endif
