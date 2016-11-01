#ifndef ARENA_H
#define ARENA_H
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <wordexp.h>
#include <unistd.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "draw_functions.h"
#include "car.h"
#include "primitives.h"
#include "colours.h"
#include "TinyXML/tinyxml.h"
using namespace std;

class Arena
{
  private:
    circle inner_circle;
    circle outer_circle;
    vector<circle> enemies;
    rectangle start;
    circle player;
    float max_attr = 0;
    float outer_radius = 0;
    float minx, miny;

    circle _read_circle(TiXmlElement* elem);
    rectangle _read_rectangle(TiXmlElement* elem);
    int _read_xml(string path);

  public:
    Arena(string path);
    void draw_arena();
    void draw_player(Car* player);
    float get_max_attr();
    float get_player_diameter();
    point get_player_position();
    void set_player_position(point);
    bool check_player_colision(point p);
    int get_num_enemies();
    vector<circle> get_enemies();
};

#endif
