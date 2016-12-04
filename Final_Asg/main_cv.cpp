#include <GL/glut.h>
#include <GL/gl.h>
#include <vector>
#include <tuple>
#include <ctime>
#include <pthread.h>
#include "Includes/car.h"
#include "Includes/arena.h"
#include "minimal_cv.h"
using namespace std;

Car* player;
Arena* arena;
GLuint etexture[2], atexture[4];
unordered_map<int,Car*> enemies;
unordered_map<string, GLfloat*> colors;
bool alive = 1, win = 0;
bool night_mode = 0;
int current_cam = 0;
char current_time[10];
int key_status[256];
float velTiro, velCarro;
int first_move = -1;
float eVelTiro, eFreqTiro, eVelCarro;
float camXY, camXZ, rbdown, c_angle, c_tip;
int detx, lastx, dety, lasty;
pthread_t csthread;

void sun() {
  GLfloat amb_white[4] = {.3, .3, .3, 0};
  GLfloat dif_white[4] = {.7, .7, .7, 1.};
  GLfloat spe_light[4] = {.5, .0, .5, 1.};
  GLfloat dir[4] = {0.3, 0.3, 1, 0};
  glLightfv(GL_LIGHT3, GL_POSITION, dir);
  glLightfv(GL_LIGHT3, GL_DIFFUSE, dif_white);
  glLightfv(GL_LIGHT3, GL_AMBIENT, amb_white);
  glLightfv(GL_LIGHT3, GL_SPECULAR, spe_light);
  glEnable(GL_LIGHTING);
}

void headlights() {
  GLfloat white[4] = { 1, 1, 1, 1};
  GLfloat purple[4] = { 1, 0, 1, 1};
  GLfloat dir[4] = {0, -1, -0.2, 0};
  GLfloat zero[4] = {0, 0.1, 0, 1};
  sp_state ps = player->get_car_state();
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(ps.position.x, ps.position.y, 0.6);
  glRotatef(ps.angle*180/M_PIl, 0, 0, 1);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, purple);
    glLightfv(GL_LIGHT2, GL_POSITION, zero);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dir);
    glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 2);

    glPushMatrix();
    glTranslatef(0.88877, -2.7, 0.18182);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
      glLightfv(GL_LIGHT0, GL_POSITION, zero);
      glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
      glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 20);
      glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0);
      glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
      glLightfv(GL_LIGHT4, GL_DIFFUSE, white);
      glLightfv(GL_LIGHT4, GL_POSITION, zero);
      glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, 2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.88877, -2.7, 0.18182);
      glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
      glLightfv(GL_LIGHT1, GL_POSITION, zero);
      glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir);
      glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20);
      glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 0);
      glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1);
      glLightfv(GL_LIGHT5, GL_DIFFUSE, white);
      glLightfv(GL_LIGHT5, GL_POSITION, zero);
      glLightf(GL_LIGHT5, GL_LINEAR_ATTENUATION, 2);
    glPopMatrix();
  glPopMatrix();

  glEnable(GL_LIGHTING);
}

void keyup(unsigned char key, int x, int y)
{
  static int toggle = 0;
  key_status[key] = 0;
  if(key == 'n' || key == 'N') {
    if(night_mode) {
      glDisable(GL_LIGHT0);
      glDisable(GL_LIGHT1);
      glDisable(GL_LIGHT2);
      glDisable(GL_LIGHT4);
      glDisable(GL_LIGHT5);
      glEnable(GL_LIGHT3);
      night_mode = 0;
      _load_texture("Textures/ceiling_texture.bmp", atexture[0]);
    } else {
      glEnable(GL_LIGHT0);
      glEnable(GL_LIGHT1);
      glEnable(GL_LIGHT2);
      glEnable(GL_LIGHT4);
      glEnable(GL_LIGHT5);
      glDisable(GL_LIGHT3);
      night_mode = 1;
      _load_texture("Textures/nightsky.bmp", atexture[0]);
    }
  }
  if(key == 'l' || key == 'L') {
    if(toggle) {
      glEnable(GL_LIGHT3);
    } else {
      glDisable(GL_LIGHT3);
    }
    toggle = !toggle;
  }
  if(key == 27) { //esc
    exit(0);
  }
  if(key == 32) { //space
    player->shoot();
  }
}

void keydown(unsigned char key, int x, int y)
{
  key_status[key] = 1;
  if(first_move < 0)
    first_move = glutGet(GLUT_ELAPSED_TIME);
}

void draw()
{
  headlights();
  sun();
  glEnable(GL_TEXTURE_2D);
  /* Draw player and enemies */
  player->draw_car();
  for(auto e: enemies)
  {
    e.second->draw_car();
  }
  /* Draw Arena */
  arena->draw_arena();
  glDisable(GL_TEXTURE_2D);
}


void third_person_cam()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, /* FOV */
                 0.8, /* aspect ratio */
                 0.1, 200.0); /* Z near and far */
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  glLoadIdentity();
  sp_state ps = player->get_car_state();
  float ca = c_angle * M_PI / 180.;
  float ct = c_tip * M_PI / 180.;
  gluLookAt(ps.position.x-1.95*sin(ps.angle),
            ps.position.y+1.95*cos(ps.angle),
            ps.position.z+2.55,  /* eye position */
            ps.position.x+60*sin(ps.angle-ca),
            ps.position.y-60*cos(ps.angle-ca),
            ps.position.z+2.55+60*sin(ct),      /* lookat */
            0.0, 0.0, 1.0);      /* up is (0, 0, 1) */
  draw();
}

void mouse_cam()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, /* FOV */
                 0.8, /* aspect ratio */
                 0.1, 200.0); /* Z near and far */
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  glLoadIdentity();
  sp_state ps = player->get_car_state();
  gluLookAt(ps.position.x + 8*sin(camXY*M_PI/180)*cos(camXZ*M_PI/180),
            ps.position.y - 8*cos(camXY*M_PI/180)*cos(camXZ*M_PI/180),
            ps.position.z + 8*sin(camXZ*M_PI/180),  /* eye position */
            ps.position.x,
            ps.position.y,
            ps.position.z,      /* lookat */
            0.0, 0.0, 1.0);     /* up is (0, 0, 1) */
  draw();
}

void first_person_cam()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, /* FOV */
                 0.8, /* aspect ratio */
                 0.1, 200.0); /* Z near and far */
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), 0.8*glutGet(GLUT_WINDOW_HEIGHT));
  glLoadIdentity();
  sp_state ps = player->get_car_state();
  gluLookAt(ps.position.x+0.5*cos(ps.angle)-0.35*sin(ps.angle),
            ps.position.y+0.35*cos(ps.angle)+0.5*sin(ps.angle),
            ps.position.z+1.45,  /* eye position */
            ps.position.x+60*sin(ps.angle),
            ps.position.y-60*cos(ps.angle),
            ps.position.z+.85,      /* lookat */
            0.0, 0.0, 1.0);      /* up is (0, 0, 1) */
  draw();

  /* Rear view Mirror */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, /* FOV */
                 0.8, /* aspect ratio */
                 0.1, 200.0); /* Z near and far */
  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0.8*glutGet(GLUT_WINDOW_HEIGHT), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
  glLoadIdentity();
  gluLookAt(ps.position.x+2.5*sin(ps.angle),
            ps.position.y-2.5*cos(ps.angle),
            ps.position.z+3.6,  /* eye position */
            ps.position.x-60*sin(ps.angle),
            ps.position.y+60*cos(ps.angle),
            ps.position.z+15,      /* lookat */
            0.0, 0.0, 1.0);      /* up is (0, 0, 1) */
  draw();
}


void minimap() {
  glPushAttrib(GL_LIGHTING_BIT|GL_DEPTH_BITS);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-40, 40, -40, 40, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0.75*glutGet(GLUT_WINDOW_WIDTH), 0, 0.25*glutGet(GLUT_WINDOW_WIDTH), 0.25*glutGet(GLUT_WINDOW_HEIGHT));
    glLoadIdentity();
    glPushAttrib(GL_CURRENT_BIT);
    arena->draw_arena_2d();
    glPopAttrib();
  glPopAttrib();
}


void hud() {
  glPushAttrib(GL_LIGHTING_BIT|GL_DEPTH_BITS);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    glLoadIdentity();
    glPushAttrib(GL_CURRENT_BIT);
      _draw_text(.9, 0.95, current_time, colors["white"]);
      if(!alive) _draw_text(.9, 0.93, "MORREU", colors["red"], GLUT_BITMAP_HELVETICA_18);
      if(win) _draw_text(.9, 0.93, "GANHOU", colors["green"], GLUT_BITMAP_HELVETICA_18);
    glPopAttrib();
  glPopAttrib();
}


void display() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(current_cam == 0) { /* third person cam */
    third_person_cam();
  } else if(current_cam == 1) { /* first person cam */
    first_person_cam();
  } else if(current_cam == 2) {
    mouse_cam(); /* mouse controlled camera */
  }

  minimap();
  hud();
  glFlush();
  glutSwapBuffers();
}

void control_cannon(int x, int y)
{
  static int lastx, lasty;
  if(x - lastx < -5) {
    c_angle = (c_angle < 45) ? c_angle + 4 : c_angle;
  } else if (x - lastx > 5) {
    c_angle = (c_angle > -45) ? c_angle - 4 : c_angle;
  }
  if(y - lasty > 5) {
    c_tip = (c_tip > 0) ? c_tip - 2 : c_tip;
  } else if (y - lasty < -5) {
    c_tip = (c_tip < 45) ? c_tip + 2 : c_tip;
  }
  c_angle = player->turn_cannon(c_angle);
  c_tip = player->tip_cannon(c_tip);
  lastx = x;
  lasty = y;
}

void control_camera(int x, int y)
{
  static int lastx, lasty;
  if(rbdown) {
    if(x > lastx) {
      camXY = (camXY < 180) ? camXY + 1 : camXY;
    } else if (x < lastx) {
      camXY = (camXY > -180) ? camXY - 1 : camXY;
    }
    if(y > lasty) {
      camXZ = (camXZ < 180) ? camXZ + 1 : camXZ;
    } else if (y < lasty) {
      camXZ = (camXZ > 0) ? camXZ - 1 : camXZ;
    }
    lastx = x;
    lasty = y;
  }
}

void shoot_cannon(int button, int state, int mx, int my)
{
  if(button == GLUT_RIGHT_BUTTON) {
    if(state == GLUT_DOWN) {
      rbdown = 1;
    } else {
      rbdown = 0;
    }
  }
}


void reshape(int w, int h)
{
  if (w > h)
      glViewport((w-h)/2., 0, (GLsizei) h, (GLsizei) h);
  else
      glViewport(0, (h-w)/2., (GLsizei) w, (GLsizei) w);
}

void idle(void) {
  static float cont = 0;
  static float last_time = glutGet(GLUT_ELAPSED_TIME);
  float elapsed_time = glutGet(GLUT_ELAPSED_TIME) - last_time;
  bool res_count = 0;
  cont += elapsed_time;

  if(alive && !win) {
    tuple<float, float> result;
    if(key_status['a'] == 1 || key_status['A'] == 1) {
      player->turn_wheel(-4);
    } if(key_status['d'] == 1 || key_status['D'] == 1) {
      player->turn_wheel(4);
  	} if(key_status['w'] == 1 || key_status['W'] == 1) {
      win = player->forward(-velCarro * elapsed_time);
  	} if(key_status['s'] == 1 || key_status['S'] == 1) {
      win = player->back(velCarro * elapsed_time);
  	} if(key_status['1'] == 1) {
      current_cam = 0;
  	} if(key_status['2'] == 1) {
      current_cam = 1;
  	} if(key_status['3'] == 1) {
      current_cam = 2;
  	}
    if(win)
      glutPostRedisplay();
    player->update_shots(velTiro * elapsed_time, &enemies, player);
    for(auto ck : enemies) {
      Car* c = ck.second;
      c->auto_forward(-eVelCarro * elapsed_time);
      if(cont >= (1./eFreqTiro)) {
        c->auto_turn_cannon();
        c->shoot();
        res_count = 1;
      } if(c->update_shots(eVelTiro * elapsed_time, &enemies, player)) {
        alive = 0;
        glutPostRedisplay();
      }
    }
    if(res_count) cont = 0;
    int counter = first_move > 0 ? glutGet(GLUT_ELAPSED_TIME) - first_move : 0;
    int seconds = counter / 1000;
    int minutes = seconds / 60;
    sprintf(current_time, "%02d:%02d:%02d", minutes, seconds%60, counter%1000);
    last_time = glutGet(GLUT_ELAPSED_TIME);
    control_cannon(detx, dety);
    glutPostRedisplay();
  }
}


string parseXML(string path)
{
  const char * const configs[9] = {"arquivoDaArena", "nome", "tipo", "caminho", "carro", "velTiro", "velCarro", "carroInimigo", "freqTiro"};
  TiXmlDocument config;
  path += "config.xml";
  config.LoadFile(path.c_str());
  TiXmlElement* root = config.FirstChildElement();
  if(!root) {
    printf("Invalid configuration file.\n");
    exit(0);
  }
  // Read configuration file
  string img;
  for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
    string elemName = elem->Value();
    if(elemName == configs[0]) {
      const char* file = elem->Attribute(configs[1]);
      const char* ext = elem->Attribute(configs[2]);
      const char* path = elem->Attribute(configs[3]);
      stringstream ss;
      ss << path << "/" << file << "." << ext;
      img = ss.str();
    } else if(elemName == configs[4]) {
      velTiro = atof(elem->Attribute(configs[5]));
      velCarro = atof(elem->Attribute(configs[6]));
    } else if(elemName == configs[7]) {
      eFreqTiro = atof(elem->Attribute(configs[8]));
      eVelTiro = atof(elem->Attribute(configs[5]));
      eVelCarro = atof(elem->Attribute(configs[6]));
    }
  }
  return img;
}

int main(int argc, char** argv) {
  // Glut stuff
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(1000,1000);
  glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-1000)/2,
                         (glutGet(GLUT_SCREEN_HEIGHT)-1000)/2);
  glutCreateWindow("Final Assignment");

  srand(time(NULL)); // Seed generator
  // Read configuration file
  if(argc < 2) {
      printf("Please provide configuration file\n");
      return 0;
  }
  string buffer = argv[1];
  string arena_file = parseXML(buffer); // Read config.xml

  // Read Arena
  glGenTextures(4, atexture);
  _load_texture("Textures/ceiling_texture.bmp", atexture[0]);
  _load_texture("Textures/floor_texture.bmp", atexture[1]);
  _load_texture("Textures/inner_wall_texture.bmp", atexture[2]);
  _load_texture("Textures/wall_texture.bmp", atexture[3]);
  arena = new Arena(arena_file, atexture);

  // Create player vehicle
  string car_file = "Obj_Parts/";
  GLuint ptexture[3], shot_texture;
  glGenTextures(3, ptexture);
  _load_texture((car_file+"tex1.bmp").c_str(), ptexture[0]);
  _load_texture((car_file+"tex2.bmp").c_str(), ptexture[1]);
  _load_texture((car_file+"tex5.bmp").c_str(), shot_texture);
  player = new Car(car_file, -1, NULL, arena, ptexture, shot_texture);

  // Create enemy vehicles
  glGenTextures(2, etexture);
  _load_texture((car_file+"tex3.bmp").c_str(), etexture[0]);
  _load_texture((car_file+"tex4.bmp").c_str(), etexture[1]);
  for(auto ek : arena->get_enemies()) {
    Car* enemy = new Car(car_file, ek.first, ek.second.color, arena, etexture, shot_texture);
    enemies[ek.first] = enemy;
  }

  // Color table
  colors = create_color_table(); //Color hash

  // Adjust constants
  float sqr = sqrt(arena->get_max_attr());
  velTiro /= sqr;
  velCarro /= sqr;
  eFreqTiro /= sqr;
  eVelTiro /= sqr;
  eVelCarro /= sqr;

  // Start thread to detect object
  int **params = (int**)malloc(2 * sizeof(int*));
  params[0] = &detx;
  params[1] = &dety;
  pthread_create(&csthread, NULL, camshift, (void*)params);

  // More glut stuff
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHT3);
  glEnable(GL_NORMALIZE);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glutDisplayFunc(display);
  glutMotionFunc(control_camera);
  glutKeyboardUpFunc(keyup);
  glutKeyboardFunc(keydown);
  glutMouseFunc(shoot_cannon);
  glutIdleFunc(idle);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;
}
