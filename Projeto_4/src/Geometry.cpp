#include "Geometry.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void Geometry::drawBox(double width, double height, double depth){
    glPushMatrix();
		glScalef(width, height, depth);
		glutSolidCube(1.0);
	glPopMatrix();
}

void Geometry::drawSolidSphere(double radius){
    glutSolidSphere(radius, 30, 30);
}

void Geometry::drawCylinder(double radius, double lenght){
    glPushMatrix();
        glRotatef(90, 1.0, 0.0, 0.0);
        auto quad = gluNewQuadric();
        //gluQuadricOrientation(quad, GLU_INSIDE);
        //gluQuadricDrawStyle( quad, GLU_FILL );
        gluCylinder(quad, radius, radius, lenght, 30, 30);
        gluDeleteQuadric(quad);
    glPopMatrix();
}
