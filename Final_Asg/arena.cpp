#include "arena.h"

circle Arena::_read_circle_unnorm(TiXmlElement* elem)
{
  circle circ;
  circ.center.x = atof(elem->Attribute("cx"));
  circ.center.y = atof(elem->Attribute("cy"));
  circ.r = atof(elem->Attribute("r"));
  return circ;
}

circle Arena::_read_circle(TiXmlElement* elem)
{
  circle circ;
  circ.center.x = atof(elem->Attribute("cx")) - minx;
  circ.center.y = maxy - atof(elem->Attribute("cy")) - miny;
  circ.r = atof(elem->Attribute("r"));
  return circ;
}

rectangle Arena::_read_rectangle(TiXmlElement* elem)
{
  rectangle rect;
  rect.origin.x = atof(elem->Attribute("x")) - minx;
  rect.origin.y = maxy - atof(elem->Attribute("y")) - miny;
  rect.h = atof(elem->Attribute("height"));
  rect.w = atof(elem->Attribute("width"));
  return rect;
}

int Arena::_read_xml(string img)
{
  const string ids[4] = {"Pista", "LargadaChegada", "Inimigo", "Jogador"};
  unordered_map<string, GLfloat*> colors = create_color_table(); //Color hash
  TiXmlDocument imagefile;
  // Expand image path shell variables
  wordexp_t img_dir;
  wordexp(img.c_str(), &img_dir, 0);
  imagefile.LoadFile(img_dir.we_wordv[0]);
  wordfree(&img_dir);

  // Open image file
  TiXmlElement* root2 = imagefile.FirstChildElement();
  if(!root2)
  {
    printf("Invalid arena file.\n");
    exit(0);
  }
  for(TiXmlElement* elem = root2->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
    string elemId = elem->Attribute("id");
    if(elemId == ids[0]) { // Track
      circle circ = _read_circle_unnorm(elem);
      if(circ.center.x + circ.r > maxx) {
        maxx = circ.center.x + circ.r;
        minx = circ.center.x - circ.r;
        maxy = circ.center.y + circ.r;
        miny = circ.center.y - circ.r;
      }
    }
  }
  // Read image file
  int enemycount = 0;
  for(TiXmlElement* elem = root2->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
    string elemId = elem->Attribute("id");
    if(elemId == ids[0]) { // Track
      circle circ = _read_circle(elem);
      circ.color = colors[elem->Attribute("fill")];
      if(circ.r > outer_radius)
      {
        outer_circle = circ;
        outer_radius = circ.r;
      } else {
        inner_circle = circ;
      }
    } else if(elemId == ids[1]) { // Start
      start = _read_rectangle(elem);
      start.color = colors[elem->Attribute("fill")];
    } else if(elemId == ids[2]) { // Enemies
      circle enemy = _read_circle(elem);
      enemy.color = colors[elem->Attribute("fill")];
      enemies[enemycount++] = enemy;
    } else if(elemId == ids[3]) { // Player
      player = _read_circle(elem);
      player.color = colors[elem->Attribute("fill")];
    }
  }
  max_attr = 2*player.r / (5.319);
  return 1;
}

Arena::Arena(string path, GLuint* textures)
{
  _read_xml(path.c_str());
  texture_ceiling = textures[0];
  texture_floor = textures[1];
  texture_inner_wall = textures[2];
  texture_wall = textures[3];
}

void Arena::draw_arena_2d()
{
  glPushMatrix();
  glTranslatef(-outer_circle.center.x/max_attr, -outer_circle.center.y/max_attr, -1);
    //Outer Ring
    glPushMatrix();
    glTranslatef(outer_circle.center.x/max_attr, outer_circle.center.y/max_attr, 0);
      _draw_circle_cont(outer_circle.r/max_attr, outer_circle.color);
    glPopMatrix();

    //Inner Ring
    glPushMatrix();
    glTranslatef(inner_circle.center.x/max_attr, inner_circle.center.y/max_attr, 0.1);
      _draw_circle_cont(inner_circle.r/max_attr, inner_circle.color);
    glPopMatrix();

    //Starting Point
    glPushMatrix();
    glTranslatef(start.origin.x/max_attr, start.origin.y/max_attr, .1);
      _draw_rectangle(start.h/max_attr, start.w/max_attr, start.color);
    glPopMatrix();

    //Player
    glPushMatrix();
    glTranslatef(player.center.x/max_attr, player.center.y/max_attr, .2);
      _draw_circle(player.r/max_attr, player.color);
    glPopMatrix();

    //Enemies
    for(auto ek : enemies)
    {
      circle e = ek.second;
      glPushMatrix();
      glTranslatef(e.center.x/max_attr, e.center.y/max_attr, 0.2);
        _draw_circle(e.r/max_attr, e.color);
      glPopMatrix();
    }
  glPopMatrix();
}

void Arena::draw_arena()
{
  glPushMatrix();
  glTranslatef(-outer_circle.center.x/max_attr, -outer_circle.center.y/max_attr, -1);
    //Outer Ring
    glPushMatrix();
    glTranslatef(outer_circle.center.x/max_attr, outer_circle.center.y/max_attr, 0);
      _draw_circle_texture(outer_circle.r/max_attr, outer_circle.color, texture_floor, 1);
      _draw_cylinder(8, outer_circle.r/max_attr, texture_wall, 1);
    glPopMatrix();

    //Inner Ring
    glPushMatrix();
    glTranslatef(inner_circle.center.x/max_attr, inner_circle.center.y/max_attr, 0.1);
      _draw_cylinder(8, inner_circle.r/max_attr, texture_inner_wall, 1);
    glPopMatrix();

    //Ceiling
    glPushMatrix();
    glTranslatef(outer_circle.center.x/max_attr, outer_circle.center.y/max_attr, 7);
      _draw_circle_texture(outer_circle.r/max_attr, outer_circle.color, texture_ceiling, -1);
    glPopMatrix();

    //Starting Point
    glPushMatrix();
    glTranslatef(start.origin.x/max_attr, start.origin.y/max_attr, .1);
      _draw_rectangle(start.h/max_attr, start.w/max_attr, start.color);
    glPopMatrix();
  glPopMatrix();
}

float Arena::get_max_attr()
{
  return max_attr;
}

float Arena::get_player_radius()
{
  return (player.r)/max_attr;
}

circ_state Arena::get_player_position()
{
  circ_state origin;
  origin.x = (player.center.x - outer_circle.center.x)/ max_attr;
  origin.y = (player.center.y - outer_circle.center.y)/ max_attr;
  origin.r = player.r / max_attr;
  return origin;
}

circ_state Arena::get_enemy_position(int id)
{
  circ_state origin;
  origin.x = (enemies.at(id).center.x - outer_circle.center.x) / max_attr;
  origin.y = (enemies.at(id).center.y - outer_circle.center.y) / max_attr;
  origin.r = enemies.at(id).r / max_attr;
  return origin;
}

void Arena::set_player_position(vec3 p)
{
  player.center.x = p.x * max_attr + outer_circle.center.x;
  player.center.y = p.y * max_attr + outer_circle.center.y;
}

void Arena::set_enemy_position(int id, vec3 p)
{
  enemies.at(id).center.x = p.x * max_attr + outer_circle.center.x;
  enemies.at(id).center.y = p.y * max_attr + outer_circle.center.y;
}

bool Arena::check_player_colision(vec3 p)
{
  float x = p.x * max_attr + outer_circle.center.x;
  float y = p.y * max_attr + outer_circle.center.y;
  float r = player.r;

  float inner_dist = sqrt(pow(x - inner_circle.center.x, 2) + pow(y - inner_circle.center.y, 2));
  float outer_dist = sqrt(pow(x - outer_circle.center.x, 2) + pow(y - outer_circle.center.y, 2));

  if(outer_dist > (outer_circle.r-r) || inner_dist < (r+inner_circle.r))
    return 0;

  for(auto ek : enemies)
  {
    circle e = ek.second;
    float dist = sqrt(pow(x - e.center.x, 2) + pow(y - e.center.y, 2));
    if(dist < (r+e.r))
      return 0;
  }
  return 1;
}

int Arena::check_player_checkpoint()
{
  float x = player.center.x;
  float y = player.center.y;

  float ox = start.origin.x;
  float oy = start.origin.y;
  float oh = start.h;
  float ow = start.w;
  if(x > ox && x < (ox + ow)) {
    if(y > oy && y < (oy + oh/2)) { // Bottom part of checkpoint
      return 2;
    } else if(y >= oy + oh/2 && y < (oy + oh)) { // Upper part of checkpoint
      return 0;
    }
  }
  return 1;
}

bool Arena::check_enemy_colision(int id, vec3 p)
{
  float x = p.x * max_attr + outer_circle.center.x;
  float y = p.y * max_attr + outer_circle.center.y;
  float r = enemies.at(id).r;

  float inner_dist = sqrt(pow(x - inner_circle.center.x, 2) + pow(y - inner_circle.center.y, 2));
  float outer_dist = sqrt(pow(x - outer_circle.center.x, 2) + pow(y - outer_circle.center.y, 2));

  if(outer_dist > (outer_circle.r-r) || inner_dist < (r+inner_circle.r))
    return 0;

  int count = 0;
  for(auto ek : enemies)
  {
    if(ek.first == id)
      continue;
    circle e = ek.second;
    float dist = sqrt(pow(x - e.center.x, 2) + pow(y - e.center.y, 2));
    if(dist < (r+e.r))
      return 0;
  }
  float dist = sqrt(pow(x - player.center.x, 2) + pow(y - player.center.y, 2));
  if(dist < (r+player.r))
    return 0;
  return 1;
}

int Arena::get_num_enemies()
{
  return enemies.size();
}

unordered_map<int,circle> Arena::get_enemies()
{
  return enemies;
}

void Arena::delete_enemy(int id)
{
  enemies.erase(id);
}
