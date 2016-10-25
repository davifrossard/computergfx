#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

void init(void)
{
	glClearColor(0., 0., 0., 0.);
	glShadeModel(GL_FLAT);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
}

void DrawAxes(double size)
{
	GLfloat mat_ambient_r[] = {1., 0., 0., 1.};
	GLfloat mat_ambient_g[] = {0., 1., 0., 1.};
	GLfloat mat_ambient_b[] = {0., 0., 1., 1.};
	GLfloat no_mat[] = {0., 0., 0., 1.};
	
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_mat);

	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_r);
		glColor3fv(mat_ambient_r);
		glScalef(size, size*0.1, size*0.1);
		glTranslatef(0.5, 0, 0);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_g);
		glColor3fv(mat_ambient_r);
		glRotatef(90, 0, 0, 1);
		glScalef(size, size*0.1, size*0.1);
		glTranslatef(0.5, 0., 0.);
		glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_EMISSION, mat_ambient_b);
		glColor3fv(mat_ambient_r);
		glRotatef(-90, 0, 1, 0);
		glScalef(size, size*0.1, size*0.1);
		glTranslatef(0.5, 0., 0.);
		glutSolidCube(1.0);
	glPopMatrix();

}

void DrawObj(double size)
{
	GLfloat materialEmission[] = {0., 0., 0., 1.};
	GLfloat materialColor[] = {0., 0., 1., 1.};
	GLfloat materialColorAMB[] = {1., 1., 0., 1.};
	GLfloat mat_specular[] = {1. , 1., 1., 1.};
	GLfloat mat_shininess[] = {128};
	
	glMaterialfv(GL_FRONT, GL_EMISSION, materialEmission);
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialColor);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialColor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glColor3f(1,0,0);

	glutSolidCube(size);
}

void display() {  

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);  
    
    GLfloat light_position[] = {0., 2., 5., 1.};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		DrawAxes(3.);
		DrawObj(1.);
	glPopMatrix();

    glutSwapBuffers(); 
}

void reshape(int w, int h)
{
	glViewport(0,0,(GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if(w<=h)
		gluPerspective(90, (GLfloat)h/(GLfloat)w, 2, 15);
	else
		gluPerspective(90, (GLfloat)w/(GLfloat)h, 2, 15);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(2,2,5,0,0,0,0,1,0);
		
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
 
    glutInitWindowSize(300, 300);
    glutCreateWindow("HUE");
    init();
 
    glutDisplayFunc(display);
	glutReshapeFunc(reshape);
    glutMainLoop();
 
    return 0;
}
