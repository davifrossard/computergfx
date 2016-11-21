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
#include <unordered_map>
#include "primitives.h"
#include "colours.h"
#include "TinyXML/tinyxml.h"
#include <cfloat>
using namespace std;

class Arena
{
  private:
    circle inner_circle;
    circle outer_circle;
    unordered_map<int, circle> enemies;
    rectangle start;
    circle player;
    float max_attr = 0;
    float outer_radius = 0;
    float minx = 0, miny = 0, maxx = 0, maxy = 0;

    circle _read_circle(TiXmlElement* elem);
    circle _read_circle_unnorm(TiXmlElement* elem);
    rectangle _read_rectangle(TiXmlElement* elem);
    int _read_xml(string path);

  public:
    Arena(string path);
    void draw_arena();
    float get_max_attr();
    float get_player_radius();
    circ_state get_player_position();
    circ_state get_enemy_position(int id);
    void set_player_position(vec3 p);
    void set_enemy_position(int id, vec3);
    bool check_player_colision(vec3 p);
    bool check_enemy_colision(int id, vec3 p);
    int get_num_enemies();
    unordered_map<int,circle> get_enemies();
    void delete_enemy(int id);
    int check_player_checkpoint();
};

#endif
