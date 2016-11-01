#ifndef CAR_H
#define CAR_H
#include <cstdio>
#include <cmath>
#include <string>
#include <tuple>
#include <GL/gl.h>
#include <GL/glut.h>
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
    float max_attr = 1, max_attr_x = 0, max_attr_y = 0;
    float wheel_angle;
    float cannon_angle;
    float wheel_mark;
    float axle_track;
    float axle_width;

    void _draw_wheel(GLfloat height, GLfloat width, GLfloat* color);
    triangle _read_triangle(TiXmlElement* elem);
    circle _read_circle(TiXmlElement* elem);
    rectangle _read_rectangle(TiXmlElement* elem);
    void _draw_front_wheels();
    void _draw_cannon();
    int _read_xml(const char* file, GLfloat* color);

  public:
    Car(string file, int id, GLfloat* color);
    void draw_car();
    float turn_wheel(float degrees);
    float get_axle_track();
    float get_axle_width();
    float turn_cannon(float degrees);
    float get_max_attr();
    float get_cannon_len();
    void forward();
    void back();
};

#endif
