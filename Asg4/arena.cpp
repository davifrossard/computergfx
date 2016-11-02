#include "arena.h"

#define normalize_x(x) (x - minx) / max_attr
#define normalize_y(y) (y - miny) / max_attr

circle Arena::_read_circle(TiXmlElement* elem)
{
  circle circ;
  circ.center.x = atof(elem->Attribute("cx"));
  circ.center.y = atof(elem->Attribute("cy"));
  circ.r = atof(elem->Attribute("r"));
  return circ;
}

rectangle Arena::_read_rectangle(TiXmlElement* elem)
{
  rectangle rect;
  rect.origin.x = atof(elem->Attribute("x"));
  rect.origin.y = atof(elem->Attribute("y"));
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
  max_attr = outer_circle.r*2;
  minx = outer_circle.center.x - outer_circle.r;
  miny = outer_circle.center.y - outer_circle.r;
  return 1;
}

Arena::Arena(string path)
{
  _read_xml(path.c_str());
}

void Arena::draw_arena()
{
  glPushMatrix();
  glTranslatef(-(outer_circle.r)/max_attr,
               -(outer_circle.r)/max_attr, 0);

    //Outer Ring
    glPushMatrix();
    glTranslatef(normalize_x(outer_circle.center.x), normalize_y(outer_circle.center.y), 0);
      _draw_circle(outer_circle.r/max_attr, outer_circle.color);
    glPopMatrix();

    //Inner Ring
    glPushMatrix();
    glTranslatef(normalize_x(inner_circle.center.x), normalize_y(inner_circle.center.y), 0);
      _draw_circle(inner_circle.r/max_attr, inner_circle.color);
    glPopMatrix();

    //Starting Point
    glPushMatrix();
    glTranslatef(normalize_x(start.origin.x), normalize_y(start.origin.y), 0);
      _draw_rectangle(start.h/max_attr, start.w/max_attr, start.color);
    glPopMatrix();

    // //Player
    // glPushMatrix();
    // glTranslatef(normalize_x(player.center.x), normalize_y(player.center.y), 0);
    //   _draw_circle(player.r/max_attr, player.color);
    // glPopMatrix();
    //
    // //Enemies
    // for(auto ek : enemies)
    // {
    //   circle e = ek.second;
    //   glPushMatrix();
    //   glTranslatef(normalize_x(e.center.x), normalize_y(e.center.y), 0);
    //     _draw_circle(e.r/max_attr, e.color);
    //   glPopMatrix();
    // }
  glPopMatrix();
}

float Arena::get_max_attr()
{
  return max_attr;
}

float Arena::get_player_radius()
{
  return (2*player.r)/max_attr;
}

point Arena::get_player_position()
{
  point origin;
  origin.x = normalize_x(player.center.x);
  origin.y = normalize_y(player.center.y);
  origin.r = player.r / max_attr;
  return origin;
}

point Arena::get_enemy_position(int id)
{
  point origin;
  origin.x = normalize_x(enemies.at(id).center.x);
  origin.y = normalize_y(enemies.at(id).center.y);
  origin.r = enemies.at(id).r / max_attr;
  return origin;
}

void Arena::set_player_position(point p)
{
  player.center.x = p.x * max_attr + minx;
  player.center.y = p.y * max_attr + miny;
}

void Arena::set_enemy_position(int id, point p)
{
  enemies.at(id).center.x = p.x * max_attr + minx;
  enemies.at(id).center.y = p.y * max_attr + miny;
}

bool Arena::check_player_colision(point p)
{
  float x = p.x * max_attr + minx;
  float y = p.y * max_attr + miny;
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

int Arena::check_player_checkpoint(point p)
{
  float x = p.x * max_attr + minx;
  float y = p.y * max_attr + miny;

  float ox = start.origin.x;
  float oy = start.origin.y;
  float oh = start.h;
  float ow = start.w;

  if(x > ox && x < (ox + ow)) {
    if(y > oy && y < (oy + oh/2)) { // Bottom part of checkpoint
      return 1;
    } else if(y >= oy + oh/2 && y < (oy + oh)) { // Upper part of checkpoint
      return 0;
    }
  }
  return 2;
}

bool Arena::check_enemy_colision(int id, point p)
{
  float x = p.x * max_attr + minx;
  float y = p.y * max_attr + miny;
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

point Arena::get_center()
{
  point origin;
  origin.x = outer_circle.r / max_attr;
  origin.y = outer_circle.r / max_attr;
  return origin;
}
