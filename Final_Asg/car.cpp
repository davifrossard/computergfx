#include "car.h"
using namespace std;


bool Car::update_shots(float inc, unordered_map<int, Car*> *car_enemies, Car* player)
{
  for (vector<projectile>::iterator p = shots.begin(); p != shots.end();)
  {
    p->origin.x -= inc * sin(p->angle);
    p->origin.y -= inc * cos(p->angle);
    p->origin.z += inc * sin(p->zangle);
    if(p->origin.x > 1000 || p->origin.x < -1000 || p->origin.y > 1000 || p->origin.y < -1000 || p->origin.z > 1000 || p->origin.z < -1000) {
      p = shots.erase(p);
    } else {
      bool deleted = 0;
      if(id == -1) { // Projectile from player
        for(auto it = car_enemies->begin(); it != car_enemies->end();) {
          Car* c = it->second;
          int key = it->first;
          float r = arena->get_enemy_position(key).r;
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
        float r = arena->get_player_radius();
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

Car::Car(string path, int id, GLfloat* color, Arena* arena, GLuint texture[2])
{
  if(color == NULL) {
    unordered_map<string, GLfloat*> colors = create_color_table(); //Color hash
    car_color = colors["green"];
  } else {
    car_color = color;
  }
  if(id == -1) {
    circ_state player_position = arena->get_player_position(); // Get Initial position
    y = player_position.y;
    x = player_position.x;
  } else {
    circ_state enemy_position = arena->get_enemy_position(id); // Get Initial position
    y = enemy_position.y;
    x = enemy_position.x;
  }
  chassis.load_obj((path+"chassis.obj").c_str());
  chassis.bind_texture(texture[0]);
  cannonbase.load_obj((path+"cannonbase.obj").c_str());
  cannonbase.bind_texture(texture[1]);
  cannon.load_obj((path+"cannon.obj").c_str());
  cannon.bind_texture(texture[1]);
  seats.load_obj((path+"seats.obj").c_str());
  seats.bind_texture(texture[1]);
  steering_wheel.load_obj((path+"steering_wheel.obj").c_str());
  steering_wheel.bind_texture(texture[1]);
  wheel.load_obj((path+"wheel.obj").c_str());
  wheel.bind_texture(texture[1]);

  this->id = id;
  this->arena = arena;
  theta = atan2(-y, -x);

  cannon_angle = 0;
  cannon_tip = 0;
  max_attr = 5.319;
  axle_width = 2.67638;
  axle_track = 2.82652;
  max_attr_x = 3.31;
  max_attr_y = 5.319;
  max_attr_z = 3.361;
}


double map_ang(double x){
  x = fmod(x + 180,360);
  if (x < 0) x += 360;
  return x - 180;
}

void Car::shoot()
{
  projectile shot;
  shot.angle = float(cannon_angle*M_PI/180 - theta);
  shot.zangle = float(cannon_tip*M_PI/180);
  shot.origin.y = y + 1.65949 * cos(theta) - 0.75 * cos(shot.angle);
  shot.origin.x = x - 1.65949 * sin(theta) - 0.75 * sin(shot.angle);
  shot.origin.z = 1.6 + 0.16 * sin(shot.zangle);
  shots.push_back(shot);
}

void Car::draw_car()
{
  for (vector<projectile>::iterator p = shots.begin(); p != shots.end(); p++)
  {
    glPushMatrix();
    glTranslatef(p->origin.x, p->origin.y, p->origin.z+0.6);
      glPushAttrib(GL_LIGHTING_BIT);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, car_color);
      _draw_point(car_color);
      glPopAttrib();
    glPopMatrix();
  }

  glPushMatrix();
  glTranslatef(x, y, 0.6);
  glRotatef(theta*180/M_PIl, 0, 0, 1);
    chassis.draw();
    glPushMatrix();
    glTranslatef(-1.34227, -1.40356, -0.85082);
    glRotatef(-wheel_angle, 0, 0, 1);
    glRotatef(-wheel_mark, 1, 0, 0);
      wheel.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.33411, -1.40356, -0.85082);
    glRotatef(-wheel_angle, 0, 0, 1);
    glRotatef(-wheel_mark, 1, 0, 0);
      wheel.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.33411, 1.42296, -0.85082);
    glRotatef(-wheel_mark, 1, 0, 0);
      wheel.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1.34227, 1.42296, -0.85082);
    glRotatef(-wheel_mark, 1, 0, 0);
      wheel.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0129, 1.65949, 0.06173);
    glRotatef(-cannon_angle, 0, 0, 1);
      cannonbase.draw();
      glTranslatef(-0.01084, -0.66588, 1.33264);
      glRotatef(-cannon_tip, 1, 0, 0);
      cannon.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.10706, 0.01334, 0.22268);
      seats.draw();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.54111, -0.45362, 0.4301);
    glRotatef(-wheel_angle, 0, 1, 0);
      steering_wheel.draw();
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
  cannon_angle = max((float)-45, min(degrees, (float)45));;
  return cannon_angle;
}

float Car::tip_cannon(float degrees)
{
  cannon_tip = max((float)0, min(degrees, (float)45));
  return cannon_tip;
}

void Car::auto_turn_cannon()
{
  circ_state p = arena->get_player_position();
  float dy = p.y - y;
  float dx = p.x - x;
  cannon_angle = -(atan2(dy, dx) - theta + M_PI/2) * 180. / M_PI;
  cannon_angle = map_ang(cannon_angle);
  if(cannon_angle > 45 || cannon_angle < -45)
    cannon_angle = (rand() % 20 - 10);
}

bool Car::forward(float inc)
{
  wheel_mark = int(wheel_mark + -5) % 360;
  float thetap = theta + inc / this->get_axle_track() * tan(wheel_angle * M_PIl/180);
  float xp = x + inc * sin(-thetap);
  float yp = y + inc * cos(-thetap);
  vec3 p = {xp,yp};
  if(arena->check_player_colision(p)) {
    theta = thetap;
    x = xp;
    y = yp;
    arena->set_player_position(p);
  }
  int cp = arena->check_player_checkpoint();
  if(last_checkpoint != cp) {
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
  if(stuck_count > (rand() % 30 + 5)) {
    sig_inc = -1;
  } else if(stuck_count < (rand() % 5)) {
    sig_inc = 1;
  }
  inc *= sig_inc;
  wheel_mark = wheel_mark > 0 ? wheel_mark - 5 : 180;

  // Calculate correct wheel angle
  float rho = sqrt(pow(y,2)+pow(x,2));
  wheel_angle = atan((float)this->get_axle_track() / rho) * 180 / M_PIl;
  float thetap = theta + inc / this->get_axle_track() * tan(-wheel_angle * M_PIl/180);
  float xp = x + inc * sin(-thetap);
  float yp = y + inc * cos(-thetap);
  vec3 p = {xp, yp};
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
  wheel_mark = int(wheel_mark + 5) % 360;
  float thetap = theta + inc / this->get_axle_track() * tan(wheel_angle * M_PIl/180);
  float xp = x + inc * sin(-thetap);
  float yp = y + inc * cos(-thetap);
  vec3 p = {xp, yp};
  if(arena->check_player_colision(p)) {
    theta = thetap;
    x = xp;
    y = yp;
    arena->set_player_position(p);
  }
  int cp = arena->check_player_checkpoint();
  if(last_checkpoint != cp) {
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

float Car::get_axle_track()
{
  return axle_track;
}

float Car::get_axle_width()
{
  return axle_width;
}

sp_state Car::get_car_state(){
  sp_state state;
  state.angle = theta;
  state.position = {x, y, 0};
  return state;
}

float Car::get_cannon_len()
{
  return 0.9;
}

float Car::get_max_attr()
{
  return max_attr;
}
