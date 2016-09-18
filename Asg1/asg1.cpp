#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string>
#include "TinyXML/tinyxml.h"
using namespace std;

GLfloat center_x, center_y, size_x, size_y;
GLfloat poly_color_r = 1, poly_color_g = 1, poly_color_b = 1;
GLfloat background_color_r = 0, background_color_g = 0, background_color_b = 0;
GLfloat win_w = 1280, win_h = 720;
GLfloat click_delta_x, click_delta_y;
enum machine_states {CREATE = 0, DRAG = 1, NONE = 2} state;
const char * const configs[] = { "janela", "largura", "altura", "fundo", "titulo", "quadrado", "tamanho", "corR", "corG", "corB"};
//                                  0          1         2         3        4           5          6         7       8       9
     
void display() {  

    glClearColor(background_color_r, background_color_g, background_color_b, 1);  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0); //Use {[0,1], [0,1], [-1, 1]} coordinate system

    if(state != CREATE) {// Dont draw the square if we're in the CREATE state.
        glBegin(GL_POLYGON);
            glColor3f(poly_color_r, poly_color_g, poly_color_b);
            glVertex3f(center_x-(size_x/2), center_y-(size_y/2), 0);
            
            glVertex3f(center_x+(size_x/2), center_y-(size_y/2), 0);
            
            glVertex3f(center_x+(size_x/2), center_y+(size_y/2), 0);
            
            glVertex3f(center_x-(size_x/2), center_y+(size_y/2), 0);
        glEnd();
    }
    
    glutSwapBuffers(); 
    
}

void mouse_drag(int mx, int my)
{
    // Adjust mouse coordinates to our system.
    float x = (float)mx/win_w; 
    float y = (float)(win_h-my)/win_h;
    if(state == DRAG) {
        //Compensate click distance from polygon center.
        center_x = x - click_delta_x;
        center_y = y - click_delta_y;
    }
    glutPostRedisplay();
}

int click_in_square(float x, float y)
{
    // Check if a click was done inside the square
    return x >= center_x-(size_x/2) && x <= center_x+(size_x/2) && y >= center_y-(size_y/2) && y <= center_y+(size_y/2);
}

void mouse_click(int btn, int st, int mx, int my)
{
    float x = (float)mx/win_w;
    float y = (float)(win_h-my)/win_h;
    if(state == CREATE && btn == 0) {// Draw square
        state = DRAG;
        center_x = x;
        center_y = y;
        glutPostRedisplay();
    } else if(click_in_square(x, y) == 1) { // Click inside square
        if(btn == 0) {// Left click, drag square
            click_delta_x = x - center_x;
            click_delta_y = y - center_y;
            state = DRAG;
        } else if(btn == 2) {// Right click, delete square
            state = CREATE;
            glutPostRedisplay();
        }
    } else { // Meaningless click
        state = NONE;
    }
    
}

int parseXML(const char* file, float& poly_size, char** title)
{
    TiXmlDocument doc;
    doc.LoadFile(file);  
    TiXmlElement* root = doc.FirstChildElement();
    if(!root)
        return 0;
    for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()) {
        string elemName = elem->Value();
        if(elemName == configs[0]) {
            for(TiXmlElement* elem2 = elem->FirstChildElement(); elem2 != NULL; elem2 = elem2->NextSiblingElement()) {
                string elemName2 = elem2->Value();
                if(elemName2 == configs[1]) {
                    win_w = atof(elem2->GetText());
                } else if(elemName2 == configs[2]) {
                    win_h = atof(elem2->GetText());
                } else if(elemName2 == configs[3]) {
                    background_color_r = atof(elem2->Attribute(configs[7]));
                    background_color_g = atof(elem2->Attribute(configs[8]));
                    background_color_b = atof(elem2->Attribute(configs[9]));
                } else if(elemName2 == configs[4]) {
                    const char* text = elem2->GetText();
                    *title = (char*)malloc(strlen(text)+1);
                    (*title)[strlen(text)] = '\0';
                    strcpy(*title, text);
                }
            }
        } else if(elemName == configs[5]) {
            poly_size = atof(elem->Attribute(configs[6]));
            poly_color_r = atof(elem->Attribute(configs[7]));
            poly_color_g = atof(elem->Attribute(configs[8]));
            poly_color_b = atof(elem->Attribute(configs[9]));
        }
    }
    return 1;
}

void reshape(int w, int h)
{
	size_x /= (GLfloat)w / win_w;
    size_y /= (GLfloat)h / win_h;
	win_w = w;
	win_h = h;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

int main(int argc, char** argv) { 
    float poly_size = 50;
    char* title = "Assignment 1";
    if(argc < 2) {
        printf("Please provide configuration file\n");
        return 0;
    }
    string buffer = argv[1];
    buffer+="/config.xml";
    if(parseXML(buffer.c_str(), poly_size, &title) == 0) {
        printf("Invalid configuration file\n");
        return 0;
    }
    size_x = (float) poly_size / win_w;
    size_y = (float) poly_size / win_h;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);    
    glutInitWindowSize(win_w,win_h);         
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-win_w)/2,
                           (glutGet(GLUT_SCREEN_HEIGHT)-win_h)/2);   
    glutCreateWindow(title); 
    
    //Callbacks
    glutDisplayFunc(display);  
    glutMouseFunc(mouse_click);
    glutMotionFunc(mouse_drag);
    glutReshapeFunc(reshape);
    
    glutMainLoop(); 
    return 0;
}
