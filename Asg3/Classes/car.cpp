#include "car.h"
#define NUM_CIRC_POLYGONS 30
#define check_max_x(x) max_attr_x = max(max_attr_x, x)
#define check_max_y(x) max_attr_y = max(max_attr_y, x)

using namespace std;


void Car::_draw_circle(GLfloat radius, GLfloat* color)
{
  GLfloat r = radius / max_attr;
  glColor3fv(color);
  glBegin( GL_TRIANGLE_FAN );
    glVertex3f(0, 0, 0);
    for(int i = 0; i <= NUM_CIRC_POLYGONS; i++)
    {
      double theta =  i * 2 * M_PIl / NUM_CIRC_POLYGONS;
      glVertex3f(r * cosf(theta), r * sinf(theta), 0);
    }
  glEnd();
}

void Car::_draw_rectangle(GLfloat height, GLfloat width, GLfloat* color)
{
  GLfloat h = height/max_attr;
  GLfloat w = width/max_attr;
  glBegin(GL_POLYGON);
    glColor3fv(color);
    glVertex3f(0, 0, 0);
    glVertex3f(0, h, 0);
    glVertex3f(w, h, 0);
    glVertex3f(w, 0, 0);
  glEnd();
}

void Car::_draw_wheel(GLfloat height, GLfloat width, GLfloat* color)
{
  _draw_rectangle(height, width, color);
  GLfloat h = height/max_attr;
  GLfloat w = width/max_attr;
  float mark_height = wheel_mark * h / 180;
  glTranslatef(0.1*w, mark_height, 0);
  glLineWidth(2.5);
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.8*w, 0, 0);
  glEnd();
}

void Car::_draw_iso_triangle(GLfloat height, GLfloat width, GLfloat* color)
{
  GLfloat h = height/max_attr;
  GLfloat w = width/max_attr;
  glBegin(GL_POLYGON);
    glColor3fv(color);
    glVertex3f(0, 0, 0);
    glVertex3f(w, 0, 0);
    glVertex3f(w/2., h, 0);
  glEnd();
}

triangle Car::_read_triangle(TiXmlElement* elem)
{
  triangle triang;
  string points = elem->Attribute("points");
  size_t pos1, pos2;
  string token;
  GLfloat x, y;
  for(int i=0;i<3;i++) {
    pos1 = points.find(" ");
    token = points.substr(0, pos1);
    pos2 = token.find(",");
    x = stof(token.substr(0, pos2));
    y = stof(token.substr(pos2+1, token.size()));
    triang.p[i].x = x;
    triang.p[i].y = y;
    check_max_x(x); check_max_y(y);
    points.erase(0, pos1 + 1);
  }
  return triang;
}

circle Car::_read_circle(TiXmlElement* elem)
{
  circle circ;
  circ.center.x = atof(elem->Attribute("cx"));
  circ.center.y = atof(elem->Attribute("cy"));
  circ.r = atof(elem->Attribute("r"));
  check_max_x(circ.center.x+circ.r); check_max_y(circ.center.y+circ.r);
  return circ;
}

rectangle Car::_read_rectangle(TiXmlElement* elem)
{
  rectangle rect;
  rect.origin.x = atof(elem->Attribute("x"));
  rect.origin.y = atof(elem->Attribute("y"));
  rect.h = atof(elem->Attribute("height"));
  rect.w = atof(elem->Attribute("width"));
  check_max_x(rect.origin.x+rect.w); check_max_y(rect.origin.y+rect.h);
  return rect;
}

void Car::_draw_front_wheels()
{
  point l_wheel_center, r_wheel_center;

  // Front Left Wheel
  l_wheel_center.x = (front_left_wheel.origin.x + front_left_wheel.w/2)/max_attr;
  l_wheel_center.y = (front_left_wheel.origin.y + front_left_wheel.h/2)/max_attr;
  glPushMatrix();
  glTranslatef(l_wheel_center.x, l_wheel_center.y, 0);
  glRotatef(wheel_angle, 0, 0, 1); // Rotate around connection joint
  glTranslatef(-front_left_wheel.w/(2*max_attr), -front_left_wheel.h/(2*max_attr), 0);
    _draw_wheel(front_left_wheel.h, front_left_wheel.w, front_left_wheel.color);
  glPopMatrix();

  // Front Left Wheel
  r_wheel_center.x = (front_right_wheel.origin.x + front_right_wheel.w/2)/max_attr;
  r_wheel_center.y = (front_right_wheel.origin.y + front_right_wheel.h/2)/max_attr;
  glPushMatrix();
  glTranslatef(r_wheel_center.x, r_wheel_center.y, 0);
  glRotatef(wheel_angle, 0, 0, 1); // Rotate around connection joint
  glTranslatef(-front_right_wheel.w/(2*max_attr), -front_right_wheel.h/(2*max_attr), 0);
    _draw_wheel(front_right_wheel.h, front_right_wheel.w, front_right_wheel.color);
  glPopMatrix();
}

void Car::_draw_cannon()
{
  point cannon_joint;
  cannon_joint.x = (cannon.origin.x + cannon.w/2)/max_attr;
  cannon_joint.y = (cannon.origin.y + cannon.h)/max_attr;

  // Cannon
  glPushMatrix();
  glTranslatef(cannon_joint.x, cannon_joint.y, 0);
  glRotatef(cannon_angle, 0, 0, 1); // Rotate around connection joint
  glTranslatef(-cannon.w/(2*max_attr), -cannon.h/max_attr, 0);
    _draw_rectangle(cannon.h, cannon.w, cannon.color);
    float delta_x = cannon_tip.center.x - cannon.origin.x;
    float delta_y = cannon_tip.center.y - cannon.origin.y;
    glTranslatef(delta_x/max_attr, delta_y/max_attr, 0);
    _draw_circle(cannon_tip.r, cannon_tip.color);
  glPopMatrix();
}

int Car::_read_xml(const char* file)
{
  unordered_map<string, GLfloat*> colors = create_color_table(); //Color hash
  TiXmlDocument config;
  config.LoadFile(file);
  TiXmlElement* car = config.FirstChildElement();
  if(!car)
      return 0;
  // Read image file
  for(TiXmlElement* elem = car->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
    string elemName = elem->Value();
    string elemId = elem->Attribute("id");
    if(elemId == "R_AXIS") { // Front axis
      rear_axis = _read_rectangle(elem);
      rear_axis.color = colors[elem->Attribute("fill")];
    } else if(elemId == "F_AXIS") { // Front axis
      front_axis = _read_rectangle(elem);
      front_axis.color = colors[elem->Attribute("fill")];
    } else if(elemId == "LF_WHEEL") { // Front axis
      front_left_wheel = _read_rectangle(elem);
      front_left_wheel.color = colors[elem->Attribute("fill")];
    } else if(elemId == "RF_WHEEL") { // Front axis
      front_right_wheel = _read_rectangle(elem);
      front_right_wheel.color = colors[elem->Attribute("fill")];
    } else if(elemId == "LR_WHEEL") { // Front axis
      rear_left_wheel = _read_rectangle(elem);
      rear_left_wheel.color = colors[elem->Attribute("fill")];
    } else if(elemId == "RR_WHEEL") { // Front axis
      rear_right_wheel = _read_rectangle(elem);
      rear_right_wheel.color = colors[elem->Attribute("fill")];
    } else if(elemId == "CANNON") { // Front axis
      cannon = _read_rectangle(elem);
      cannon.color = colors[elem->Attribute("fill")];
    } else if(elemId == "CHASSIS") { // Front axis
      chassis = _read_rectangle(elem);
      chassis.color = colors[elem->Attribute("fill")];
    } else if(elemId == "SPOILER") { // Front axis
      spoiler = _read_rectangle(elem);
      spoiler.color = colors[elem->Attribute("fill")];
    } else if(elemId == "COCKPIT") { // Front axis
      cockpit = _read_triangle(elem);
      cockpit.color = colors[elem->Attribute("fill")];
    } else if(elemId == "CANNON_TIP") { // Front axis
      cannon_tip = _read_circle(elem);
      cannon_tip.color = colors[elem->Attribute("fill")];
    } else if(elemId == "DRIVER") { // Front axis
      driver = _read_circle(elem);
      driver.color = colors[elem->Attribute("fill")];
    } else {
      printf("Unknown ID: %s\n", elemId.c_str());
    }
  }
  max_attr = max(max_attr_x, max_attr_y);
}

Car::Car(string file)
{
  _read_xml(file.c_str());
}

void Car::draw_car()
{
  glPushMatrix();
  glTranslatef(-max_attr_x/(2*max_attr), -max_attr_y/(2*max_attr), 0);
    // Front Axis
    glPushMatrix();
    glTranslatef(front_axis.origin.x/max_attr, front_axis.origin.y/max_attr, 0);
      _draw_rectangle(front_axis.h, front_axis.w, front_axis.color);
    glPopMatrix();

    // Rear Axis
    glPushMatrix();
    glTranslatef(rear_axis.origin.x/max_attr, rear_axis.origin.y/max_attr, 0);
      _draw_rectangle(rear_axis.h, rear_axis.w, rear_axis.color);
    glPopMatrix();

    // Draw Front Wheels
    _draw_front_wheels();

    // Draw Cannon
    _draw_cannon();

    // Rear Left Wheel
    glPushMatrix();
    glTranslatef(rear_left_wheel.origin.x/max_attr, rear_left_wheel.origin.y/max_attr, 0);
      _draw_wheel(rear_left_wheel.h, rear_left_wheel.w, rear_left_wheel.color);
    glPopMatrix();

    // Rear Right Wheel
    glPushMatrix();
    glTranslatef(rear_right_wheel.origin.x/max_attr, rear_right_wheel.origin.y/max_attr, 0);
      _draw_wheel(rear_right_wheel.h, rear_right_wheel.w, rear_right_wheel.color);
    glPopMatrix();

    // Cockpit
    glPushMatrix();
    glTranslatef(cockpit.p[1].x/max_attr, cockpit.p[1].y/max_attr, 0);
      GLfloat w = (cockpit.p[0].x - cockpit.p[1].x);
      GLfloat h = -(cockpit.p[0].y - cockpit.p[2].y);
      _draw_iso_triangle(h, w, cockpit.color);
    glPopMatrix();

    // Chassis
    glPushMatrix();
    glTranslatef(chassis.origin.x/max_attr, chassis.origin.y/max_attr, 0);
      _draw_rectangle(chassis.h, chassis.w, chassis.color);
    glPopMatrix();

    // Spoiler
    glPushMatrix();
    glTranslatef(spoiler.origin.x/max_attr, spoiler.origin.y/max_attr, 0);
      _draw_rectangle(spoiler.h, spoiler.w, spoiler.color);
    glPopMatrix();

    // Driver
    glPushMatrix();
    glTranslatef(driver.center.x/max_attr, driver.center.y/max_attr, 0);
      _draw_circle(driver.r, driver.color);
    glPopMatrix();
  glPopMatrix();
}

void Car::turn_wheel(float degrees)
{
  wheel_angle += degrees;
  wheel_angle = max((float)-45, min(wheel_angle, (float)45));
}

void Car::turn_cannon(float degrees)
{
  cannon_angle = degrees;
}

void Car::forward()
{
  wheel_mark = wheel_mark > 0 ? wheel_mark - 5 : 180;
  printf ("%f\n", wheel_mark);
}

void Car::back()
{
  wheel_mark = wheel_mark < 180 ? wheel_mark + 5 : 0;
}
