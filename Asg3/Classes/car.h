#ifndef CAR_H
#define CAR_H
#include <cstdio>
#include <cmath>
#include <string>
#include <GL/gl.h>
#include <GL/glut.h>
#include "primitives.h"
#include "../colours.h"
#include "../TinyXML/tinyxml.h"
using namespace std;

class Car
{
  private:
    rectangle rear_axis;
    rectangle rear_left_wheel;
    rectangle rear_right_wheel;
    rectangle front_axis;
    rectangle front_left_wheel;
    rectangle front_right_wheel;
    rectangle chassis;
    rectangle spoiler;
    circle driver;
    triangle cockpit;
    rectangle cannon;
    circle cannon_tip;
    float max_attr = 1, max_attr_x = 0, max_attr_y = 0;
    float wheel_angle;
    float cannon_angle;
    float wheel_mark;

    void _draw_circle(GLfloat radius, GLfloat* color);
    void _draw_rectangle(GLfloat height, GLfloat width, GLfloat* color);
    void _draw_wheel(GLfloat height, GLfloat width, GLfloat* color);
    void _draw_iso_triangle(GLfloat height, GLfloat width, GLfloat* color);
    triangle _read_triangle(TiXmlElement* elem);
    circle _read_circle(TiXmlElement* elem);
    rectangle _read_rectangle(TiXmlElement* elem);
    void _draw_front_wheels();
    void _draw_cannon();
    int _read_xml(const char* file);

  public:
    Car(string file);
    void draw_car();
    void turn_wheel(float degrees);
    void turn_cannon(float degrees);
    void forward();
    void back();
};

#endif
