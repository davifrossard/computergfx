#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>   
#include <math.h>

#define paddleHeight 80
#define paddleWidth 10
#define baseHeight 40
#define baseWidth 100
#define dariusWheel 30
#define zAxis 0
const int Width = 700;
const int Height = 700;
int key_status[256];
GLfloat gx, gy, gwheel, gtheta1, gtheta2, gtheta3;

void drawRect(int height, int width, float R, float G, float B)
{
	glColor3f(R, G, B);
	glBegin(GL_QUADS);
		glVertex2f(-width/2.0, 0.0);
		glVertex2f(-width/2.0, height);
		glVertex2f(width/2.0, height);
		glVertex2f(width/2.0, 0.0);
	glEnd();
}

void drawCirc(int r, float R, float G, float B)
{
	glPushMatrix();	
	glRotatef(gwheel, 0., 0., 1.);
	glColor3f(R, G, B);
	glPointSize(2);
	glBegin(GL_POINTS);
	int i=0;
	for(i=0; i<=360; i+= 20)
		glVertex3f(r*cos(M_PI*i/180), r*sin(M_PI*i/180), 0); 
	glEnd();
	glPopMatrix();
}

void desenhaRobo(GLfloat x, GLfloat y, GLfloat Wtheta, GLfloat A1theta, GLfloat A2theta, GLfloat A3theta)
{
	glTranslatef(gx, gy, 0.);
	drawRect(baseHeight, baseWidth, 1., 0., 0.);
	glPushMatrix();
	glTranslatef(-baseWidth/3., 0., 0.);
	drawCirc(20, 1, 1, 1);
	glPopMatrix();
	glPushMatrix();	
	glTranslatef(baseWidth/3., 0., 0.);
	drawCirc(20, 1, 1, 1);
	glPopMatrix();

	glTranslatef(0., baseHeight, 0.);
	glRotatef(gtheta1, 0., 0., 1.);
	drawRect(paddleHeight, paddleWidth, 0., 0., 1.);
	glTranslatef(0., paddleHeight, 0.);
	glRotatef(gtheta2, 0., 0., 1.);
	drawRect(paddleHeight, paddleWidth, 0., 1., 1.);
	glTranslatef(0., paddleHeight, 0.);
	glRotatef(gtheta3, 0., 0., 1.);
	drawRect(paddleHeight, paddleWidth, 0., 1., 0.);
	glPopMatrix();
}

void display() {  

    glClearColor(0, 0, 0, 0);  
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	desenhaRobo(gx, gy, gwheel, gtheta1, gtheta2, gtheta3);
	glPopMatrix();

    glutSwapBuffers(); 
    
}

void keyup(unsigned char key, int x, int y)
{
    key_status[key] = 0;
    int i;
}


void keydown(unsigned char key, int x, int y)
{
    key_status[key] = 1;
    int i;
}

void moveY(float delta_y)
{
	gy += delta_y;
}

void moveX(float delta_x)
{
	gx += delta_x;
	gwheel -= delta_x * 360 / (2 * M_PI * 20);
}

void idle(void) {
    float inc = 1.0;
	float degree_inc = 1;
    if(key_status['w'] == 1 || key_status['W'] == 1) {
        moveY(inc);
    } if(key_status['s'] == 1 || key_status['S'] == 1) {
        moveY(-inc);
    } if(key_status['a'] == 1 || key_status['A'] == 1) {
        moveX(-inc);
    } if(key_status['d'] == 1 || key_status['D'] == 1) {
        moveX(inc);
	} if(key_status['r'] == 1 || key_status['R'] == 1) {
        gtheta1 += degree_inc;
	} if(key_status['f'] == 1 || key_status['F'] == 1) {
        gtheta1 -= degree_inc;
	} if(key_status['t'] == 1 || key_status['T'] == 1) {
        gtheta2 += degree_inc;
	} if(key_status['g'] == 1 || key_status['G'] == 1) {
        gtheta2 -= degree_inc;
	} if(key_status['y'] == 1 || key_status['Y'] == 1) {
        gtheta3 += degree_inc;
	} if(key_status['h'] == 1 || key_status['h'] == 1) {
        gtheta3 -= degree_inc;
	}    
	glutPostRedisplay();
}


int main(int argc, char** argv) { 
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);    
    glutInitWindowSize(Width,Height);         
    glutInitWindowPosition(100,100);     
    glutCreateWindow("Robot"); 

    glutDisplayFunc(display);  
    glutKeyboardUpFunc(keyup);
    glutKeyboardFunc(keydown);
    glutIdleFunc(idle);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glOrtho(-250.0, 250.0, -250.0, 250.0, -1.0, 1.0);
    
    glutMainLoop(); 
    return 0;

}
