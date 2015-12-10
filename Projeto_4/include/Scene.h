#ifndef SCENE_H
#define SCENE_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Person.h"
#include "Vector3.h"

class Scene
{
    public:
        static void reshape(int width, int height);
        static void draw();

        static void animation(int t);
        static void initGL();
        static void setWindowSize(int width,int height);

        static void drawPlane();

        static void keyboardDown(unsigned char key, int x, int y);
        static void keyboardSpecialDown(int key, int x, int y);
        static void keyboardSpecialUp(int key, int x, int y);

    public:
        static GLint FPS;
        static bool KEYS[256];

    private:
        static Vector3 m_viewPos;
        static int m_zoom;

        /*Default lighting*/
		static GLfloat m_lightDiffuse0[4];
		static GLfloat m_ambientLight[4];
		static GLfloat m_lightPositio0[4];

		/* size of the window */
		static int m_windowSizeX;
		static int m_windowSizeY;

		/* Tick */
		static int m_tick;

		/* Person */
		static Person m_person;
};

#endif // SCENE_H
