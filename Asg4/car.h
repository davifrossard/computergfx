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
using namespace std;

class Car
{
  private:
    rectangle rear_axle;
    rectangle rear_left_wheel;
    rectangle rear_right_wheel;
    rectangle front_axle;
    rectangle front_left_wheel;
    rectangle front_right_wheel;
    rectangle chassis;
    rectangle spoiler;
    circle driver;
    triangle cockpit;
    rectangle cannon;
    circle cannon_tip;
    int id;
    GLfloat* car_color;
    GLfloat x, y, theta = 0;
    GLfloat cx, cy;
    float max_attr = 1, max_attr_x = 0, max_attr_y = 0;
    float wheel_angle, cannon_angle;
    float axle_track, axle_width;
    float wheel_mark;
    Arena* arena;
    float car_to_arena;
    vector<projectile> shots;
    int stuck_count = 0, sig_inc = 1;
    int score = 0, last_checkpoint = 0, expected_checkpoint = 1;

    void _draw_wheel(GLfloat height, GLfloat width, GLfloat* color);
    triangle _read_triangle(TiXmlElement* elem);
    circle _read_circle(TiXmlElement* elem);
    rectangle _read_rectangle(TiXmlElement* elem);
    void _draw_front_wheels();
    void _draw_cannon();
    int _read_xml(const char* file, GLfloat* color);

  public:
    Car(string file, int id, GLfloat* color, Arena* arena);
    void draw_car();
    float get_axle_track();
    float get_axle_width();
    float get_max_attr();
    float get_cannon_len();
    float turn_cannon(float degrees);
    float turn_wheel(float degrees);
    bool forward(float amount);
    void auto_forward(float amount);
    void auto_turn_cannon();
    bool back(float amount);
    void shoot();
    bool update_shots(float inc, unordered_map<int, Car*> *car_enemies, Car* player);
};

#endif
