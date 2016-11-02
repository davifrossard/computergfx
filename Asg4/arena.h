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
    float minx, miny;

    circle _read_circle(TiXmlElement* elem);
    rectangle _read_rectangle(TiXmlElement* elem);
    int _read_xml(string path);

  public:
    Arena(string path);
    void draw_arena();
    float get_max_attr();
    float get_player_radius();
    point get_player_position();
    point get_center();
    point get_enemy_position(int id);
    void set_player_position(point);
    void set_enemy_position(int id, point);
    bool check_player_colision(point p);
    bool check_enemy_colision(int id, point p);
    int get_num_enemies();
    unordered_map<int,circle> get_enemies();
    void delete_enemy(int id);
    int check_player_checkpoint(point p);
};

#endif
