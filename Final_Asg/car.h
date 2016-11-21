#ifndef CAR_H
#define CAR_H
#include <cstdio>
#include <cmath>
#include <string>
#include <tuple>
#include <GL/gl.h>
#include <GL/glut.h>
#include "arena.h"
#include "draw_functions.h"
#include "primitives.h"
#include "colours.h"
#include "TinyXML/tinyxml.h"
#include "Loader/MeshObject.h"
using namespace std;

class Car
{
  private:
    MeshObject chassis;
    MeshObject cannonbase;
    MeshObject cannon;
    MeshObject seats;
    MeshObject steering_wheel;
    MeshObject wheel;
    GLuint texture[2];

    int id;
    GLfloat* car_color;
    GLfloat x, y, theta = 0;
    float max_attr = 1, max_attr_x = 0, max_attr_y = 0, max_attr_z = 0;
    float wheel_angle, cannon_angle, cannon_tip;
    float axle_track, axle_width;
    float wheel_mark;
    Arena* arena;
    vector<projectile> shots;
    int stuck_count = 0, sig_inc = 1;
    int score = 0, last_checkpoint = 0, expected_checkpoint = 1;

  public:
    Car(string path, int id, GLfloat* color, Arena* arena, GLuint texture[2]);
    void draw_car();
    float get_axle_track();
    float get_axle_width();
    float get_max_attr();
    float get_cannon_len();
    float turn_cannon(float degrees);
    float tip_cannon(float degrees);
    float turn_wheel(float degrees);
    bool forward(float amount);
    void auto_forward(float amount);
    void auto_turn_cannon();
    bool back(float amount);
    void shoot();
    bool update_shots(float inc, unordered_map<int, Car*> *car_enemies, Car* player);
    sp_state get_car_state();
};

#endif
