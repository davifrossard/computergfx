#include "car.h"
#define check_max_x(x) max_attr_x = max(max_attr_x, x)
#define check_max_y(x) max_attr_y = max(max_attr_y, x)

using namespace std;


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

bool Car::update_shots(float inc, unordered_map<int, Car*> *car_enemies, Car* player)
{
  for (vector<projectile>::iterator p = shots.begin(); p != shots.end();)
  {
    p->origin.x += inc * sin(p->angle);
    p->origin.y -= inc * cos(p->angle);
    float px = p->origin.x * car_to_arena;
    float py = p->origin.y * car_to_arena;
    if(px > 1 || px < 0 || py > 1 || py < 0)
      p = shots.erase(p);
    else {
      bool deleted = 0;
      if(id == -1) { // Projectile from player
        for(auto it = car_enemies->begin(); it != car_enemies->end();) {
          Car* c = it->second;
          int key = it->first;
          float r = arena->get_enemy_position(key).r / car_to_arena;
          float dist = sqrt(pow(c->x - p->origin.x, 2) + pow(c->y - p->origin.y, 2));
          if(dist<r)
          {
            it = car_enemies->erase(it);
            arena->delete_enemy(key);
            p = shots.erase(p);
            deleted = 1;
            break;
          }
          else
            it++;
        }
      } else { // Projectile from enemy
        float r = arena->get_player_radius() / (2 * car_to_arena);
        float dist = sqrt(pow(player->x - p->origin.x, 2) + pow(player->y - p->origin.y, 2));
        if(dist<r)
          return 1;
      }
    if(deleted == 0)
      p++;
    }
  }
  return 0;
}
void Car::_draw_wheel(GLfloat height, GLfloat width, GLfloat* color)
{
  _draw_rectangle(height, width, color);
  GLfloat h = height;
  GLfloat w = width;
  float mark_height = wheel_mark * h / 180;
  glTranslatef(0.1*w, mark_height, 0);
  glLineWidth(5);
  glColor3fv(car_color);
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.8*w, 0, 0);
  glEnd();
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
    _draw_wheel(front_left_wheel.h/max_attr, front_left_wheel.w/max_attr, front_left_wheel.color);
  glPopMatrix();

  // Front Left Wheel
  r_wheel_center.x = (front_right_wheel.origin.x + front_right_wheel.w/2)/max_attr;
  r_wheel_center.y = (front_right_wheel.origin.y + front_right_wheel.h/2)/max_attr;
  glPushMatrix();
  glTranslatef(r_wheel_center.x, r_wheel_center.y, 0);
  glRotatef(wheel_angle, 0, 0, 1); // Rotate around connection joint
  glTranslatef(-front_right_wheel.w/(2*max_attr), -front_right_wheel.h/(2*max_attr), 0);
    _draw_wheel(front_right_wheel.h/max_attr, front_right_wheel.w/max_attr, front_right_wheel.color);
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
    _draw_rectangle(cannon.h/max_attr, cannon.w/max_attr, cannon.color);
    float delta_x = cannon_tip.center.x - cannon.origin.x;
    float delta_y = cannon_tip.center.y - cannon.origin.y;
    glTranslatef(delta_x/max_attr, delta_y/max_attr, 0);
    _draw_circle(cannon_tip.r/max_attr, cannon_tip.color);
  glPopMatrix();
}

int Car::_read_xml(const char* file, GLfloat* color)
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
      rear_axle = _read_rectangle(elem);
      rear_axle.color = color;
    } else if(elemId == "F_AXIS") { // Front axis
      front_axle = _read_rectangle(elem);
      front_axle.color = color;
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
      cockpit.color = color;
    } else if(elemId == "CANNON_TIP") { // Front axis
      cannon_tip = _read_circle(elem);
      cannon_tip.color = colors[elem->Attribute("fill")];
    } else if(elemId == "DRIVER") { // Front axis
      driver = _read_circle(elem);
      driver.color = color;
    } else {
      printf("Unknown ID: %s\n", elemId.c_str());
    }
  }
  max_attr = max(max_attr_x, max_attr_y);
  axle_width = front_axle.w;
  axle_track = abs(front_axle.origin.y - rear_axle.origin.y);
  car_color = color;

}

Car::Car(string file, int id, GLfloat* color, Arena* arena)
{
  if(color == NULL)
  {
    unordered_map<string, GLfloat*> colors = create_color_table(); //Color hash
    color = colors["green"];
  }
  _read_xml(file.c_str(), color);
  this->id = id;
  this->arena = arena;
  float car_to_arena = arena->get_player_radius();
  this->car_to_arena = car_to_arena;
  if(id == -1) {
    point player_position = arena->get_player_position(); // Get Initial position
    y = player_position.y / car_to_arena;
    x = player_position.x / car_to_arena; // Set player initial position
  } else {
    point enemy_position = arena->get_enemy_position(id); // Get Initial position
    y = enemy_position.y / car_to_arena;
    x = enemy_position.x / car_to_arena; // Set player initial position
  }
  point arena_center = arena->get_center();
  cx = arena_center.x / car_to_arena; // Get arena center
  cy = arena_center.y / car_to_arena;
  theta = atan2(y-cy, x-cx);
  wheel_mark = 0;
}

void Car::shoot()
{
  projectile shot;
  shot.origin.x = x-this->get_cannon_len()*sin(theta);
  shot.origin.y = y+this->get_cannon_len()*cos(theta);
  shot.angle = theta + cannon_angle*M_PIl/180;
  shots.push_back(shot);
}

void Car::draw_car()
{
  for (vector<projectile>::iterator p = shots.begin(); p != shots.end(); p++)
  {
    glPushMatrix();
    glTranslatef(p->origin.x, p->origin.y, 0);
    glRotatef(p->angle*180/M_PIl, 0, 0, 1);
      _draw_point(car_color);
    glPopMatrix();
  }
  glPushMatrix();
  glTranslatef(x, y, 0);
  glRotatef(theta*180/M_PIl, 0, 0, 1);
  glTranslatef(-max_attr_x/(2*max_attr), -max_attr_y/(2*max_attr), 0);
    // Front Axis
    glPushMatrix();
    glTranslatef(front_axle.origin.x/max_attr, front_axle.origin.y/max_attr, 0);
      _draw_rectangle(front_axle.h/max_attr, front_axle.w/max_attr, front_axle.color);
    glPopMatrix();

    // Rear Axis
    glPushMatrix();
    glTranslatef(rear_axle.origin.x/max_attr, rear_axle.origin.y/max_attr, 0);
      _draw_rectangle(rear_axle.h/max_attr, rear_axle.w/max_attr, rear_axle.color);
    glPopMatrix();

    // Draw Front Wheels
    _draw_front_wheels();

    // Draw Cannon
    _draw_cannon();

    // Rear Left Wheel
    glPushMatrix();
    glTranslatef(rear_left_wheel.origin.x/max_attr, rear_left_wheel.origin.y/max_attr, 0);
      _draw_wheel(rear_left_wheel.h/max_attr, rear_left_wheel.w/max_attr, rear_left_wheel.color);
    glPopMatrix();

    // Rear Right Wheel
    glPushMatrix();
    glTranslatef(rear_right_wheel.origin.x/max_attr, rear_right_wheel.origin.y/max_attr, 0);
      _draw_wheel(rear_right_wheel.h/max_attr, rear_right_wheel.w/max_attr, rear_right_wheel.color);
    glPopMatrix();

    // Cockpit
    glPushMatrix();
    glTranslatef(cockpit.p[1].x/max_attr, cockpit.p[1].y/max_attr, 0);
      GLfloat w = (cockpit.p[0].x - cockpit.p[1].x);
      GLfloat h = -(cockpit.p[0].y - cockpit.p[2].y);
      _draw_iso_triangle(h/max_attr, w/max_attr, cockpit.color);
    glPopMatrix();

    // Chassis
    glPushMatrix();
    glTranslatef(chassis.origin.x/max_attr, chassis.origin.y/max_attr, 0);
      _draw_rectangle(chassis.h/max_attr, chassis.w/max_attr, chassis.color);
    glPopMatrix();

    // Spoiler
    glPushMatrix();
    glTranslatef(spoiler.origin.x/max_attr, spoiler.origin.y/max_attr, 0);
      _draw_rectangle(spoiler.h/max_attr, spoiler.w/max_attr, spoiler.color);
    glPopMatrix();

    // Driver
    glPushMatrix();
    glTranslatef(driver.center.x/max_attr, driver.center.y/max_attr, 0);
      _draw_circle(driver.r/max_attr, driver.color);
    glPopMatrix();
  glPopMatrix();
}

float Car::turn_wheel(float degrees)
{
  wheel_angle += degrees;
  wheel_angle = max((float)-45, min(wheel_angle, (float)45));
  return wheel_angle;
}

float Car::turn_cannon(float degrees)
{
  cannon_angle = degrees;
  return cannon_angle;
}

double map_ang(double x){
  x = fmod(x + 180,360);
  if (x < 0) x += 360;
  return x - 180;
}

void Car::auto_turn_cannon()
{
  point p = arena->get_player_position();
  float dy = y - p.y / car_to_arena;
  float dx = x - p.x / car_to_arena;
  cannon_angle = (atan2(dy, dx) - theta) * 180. / M_PIl - (rand() % 20 + 80);
  cannon_angle = map_ang(cannon_angle);
  if(cannon_angle > 45 || cannon_angle < -45)
    cannon_angle = (rand() % 90 - 45);
}

bool Car::forward(float inc)
{
  wheel_mark = wheel_mark > 0 ? wheel_mark - 5 : 180;
  float thetap = theta + inc / this->get_axle_track() * tan(-wheel_angle * M_PIl/180);
  float xp = x + inc * sin(-thetap);
  float yp = y + inc * cos(-thetap);
  point p = {xp * car_to_arena, yp * car_to_arena};
  if(arena->check_player_colision(p))
  {
    theta = thetap;
    x = xp;
    y = yp;
    arena->set_player_position(p);
  }
  int cp = arena->check_player_checkpoint(p);
  if(last_checkpoint != cp)
  {
    last_checkpoint = cp;
    if(cp == expected_checkpoint)
    {
      score++;
      expected_checkpoint = (expected_checkpoint + 1) % 3;
    } else {
      score--;
    }
  }

  if(score == 5)
    return 1;
  else
    return 0;
}

void Car::auto_forward(float inc)
{
  if(stuck_count == (rand() % 10 + 5)) {
    sig_inc = -1;
  } else if(stuck_count == (rand() % 5)) {
    sig_inc = 1;
  }
  inc *= sig_inc;
  wheel_mark = wheel_mark > 0 ? wheel_mark - 5 : 180;

  // Calculate correct wheel angle
  float rho = sqrt(pow(y-cy,2)+pow(x-cx,2));
  wheel_angle = -atan((float)this->get_axle_track() / rho) * 180 / M_PIl;
  float thetap = theta + inc / this->get_axle_track() * tan(-wheel_angle * M_PIl/180);
  float xp = x + inc * sin(-thetap);
  float yp = y + inc * cos(-thetap);
  point p = {xp * car_to_arena, yp * car_to_arena};
  if(arena->check_enemy_colision(id, p)) {
    theta = thetap;
    x = xp;
    y = yp;
    arena->set_enemy_position(id, p);
    if(stuck_count > 0)
      stuck_count--;
  } else {
    if(stuck_count < 10)
      stuck_count++;
  }
}

bool Car::back(float inc)
{
  wheel_mark = wheel_mark < 180 ? wheel_mark + 5 : 0;
  float thetap = theta + inc / this->get_axle_track() * tan(-wheel_angle * M_PIl/180);
  float xp = x + inc * sin(-thetap);
  float yp = y + inc * cos(-thetap);
  point p = {xp * car_to_arena, yp * car_to_arena};
  if(arena->check_player_colision(p)) {
    theta = thetap;
    x = xp;
    y = yp;
    arena->set_player_position(p);
  }
  int cp = arena->check_player_checkpoint(p);
  if(last_checkpoint != cp)
  {
    last_checkpoint = cp;
    if(cp == expected_checkpoint)
    {
      score++;
      expected_checkpoint = (expected_checkpoint + 1) % 3;
    } else {
      score--;
      expected_checkpoint = (expected_checkpoint - 1) % 3;
    }
  }
  if(score == 5)
    return 1;
  else
    return 0;
}

float Car::get_axle_track()
{
  return axle_track/max_attr;
}

float Car::get_axle_width()
{
  return axle_width/max_attr;
}

float Car::get_cannon_len()
{
  return (chassis.origin.y - driver.center.y )/max_attr;
}

float Car::get_max_attr()
{
  return max_attr;
}
