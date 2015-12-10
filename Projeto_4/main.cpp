// Para rodar no Windows descomente a linha abaixo!
//#include <windows.h>

#include <stdlib.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "Scene.h"

// Para instalar o glut e opengl:
//	 sudo apt-get install freeglut3-dev
// Compile e execute usando:
//	 make
//	 ./exec
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 480);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Projeto 4 - CG");

	glutReshapeFunc(Scene::reshape);
	glutDisplayFunc(Scene::draw);

	glutKeyboardFunc(Scene::keyboardDown);
	glutSpecialFunc(Scene::keyboardSpecialDown);
	glutSpecialUpFunc(Scene::keyboardSpecialUp);

	glutTimerFunc((int) 1000/Scene::FPS, Scene::animation, 0);

	//glutIgnoreKeyRepeat(false); // process keys held down

	Scene::initGL();

	glutMainLoop();
	return 0;
}
