#include "Scene.h"

/* Variaveis estaticas */
GLint Scene::FPS = 10;
bool Scene::KEYS[256];

// visao geral = 0.0, -2.5, 0.0;
// visao do chao = 0.0, -0.009, 0.0;
Vector3 Scene::m_viewPos = Vector3(0.0, -2.5, 0.0);

int Scene::m_zoom = -5;

GLfloat Scene::m_lightDiffuse0[4];
GLfloat Scene::m_ambientLight[4];
GLfloat Scene::m_lightPositio0[4];

int Scene::m_windowSizeX;
int Scene::m_windowSizeY;

int Scene::m_tick = 0;

Person Scene::m_person{m_tick};

/* Funções */
void Scene::reshape(int width, int height){
    m_windowSizeX = width;
	m_windowSizeY = height;

	// Determine the new aspect ratio
	GLdouble gldAspect = (GLdouble) width / (GLdouble) height;

	// Reset the projection matrix with the new aspect ratio.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(40.0, gldAspect, 0.01, 60.0);
	gluPerspective(80.0, gldAspect, 1.0, 500.0);
	glTranslatef( m_viewPos.x, m_viewPos.y, m_viewPos.z+m_zoom );

	// Set the viewport to take up the entire window.
	glViewport(0, 0, width, height);
}

void Scene::draw(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_LIGHTING);

    drawPlane();
    m_person.draw(m_tick);

	glutSwapBuffers();
}

void Scene::animation(int t){
    bool keyPressed = false;
    if(KEYS[GLUT_KEY_UP]){
        m_tick++;
        m_person.movingFoward();
        m_person.updatePos();
        keyPressed = true;
    }else if(KEYS[GLUT_KEY_DOWN]){
        m_tick--;
        m_person.movingBack();
        m_person.updatePos();
        keyPressed = true;
    }

    if(KEYS[GLUT_KEY_LEFT]){
        m_person.addRotation(15);
        keyPressed = true;
    }else if(KEYS[GLUT_KEY_RIGHT]){
        m_person.addRotation(-15);
        keyPressed = true;
    }

    if(keyPressed)
        glutPostRedisplay();
    glutTimerFunc((int) 1000/Scene::FPS, Scene::animation, 0);
}

void Scene::initGL(){
	/*Default lighting*/
	m_lightDiffuse0[0] = 1.0;
	m_lightDiffuse0[1] = 1.0;
	m_lightDiffuse0[2] = 1.0;
	m_lightDiffuse0[3] = 1.0;

	m_lightPositio0[0] = 1.0;
	m_lightPositio0[1] = 50.0;
	m_lightPositio0[2] = 5.0;
	m_lightPositio0[3] = 0.0;

	m_ambientLight[0] = 0.5;
    m_ambientLight[1] = 0.5;
    m_ambientLight[2] = 0.5;
    m_ambientLight[3] = 1.0;

	glEnable(GL_LIGHTING);

    glLightfv (GL_LIGHT0, GL_DIFFUSE,	m_lightDiffuse0);
	glLightfv (GL_LIGHT0, GL_AMBIENT, 	m_ambientLight);
	glLightfv (GL_LIGHT0, GL_POSITION,	m_lightPositio0);

	glEnable(GL_LIGHT0);

	/* size of the window */
	m_windowSizeX = 800;
	m_windowSizeY = 480;

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	reshape(m_windowSizeX, m_windowSizeY);

	glClearColor(0.126f, 0.126f, 0.128f, 1.0f);
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);
}

void Scene::setWindowSize(int width,int height){
    m_windowSizeX = width;
    m_windowSizeY = height;
}

void Scene::keyboardDown(unsigned char key, int x, int y){

	switch(key){
        case 'Q': //termina o programa
        case 'q':
        case  27:   // ESC
            exit(0);
            break;
        case '+'://aumenta o zoom
            m_zoom += 1;
            reshape(m_windowSizeX, m_windowSizeY);
            break;
        case '-'://diminui o zoom
            m_zoom -= 1;
            reshape(m_windowSizeX, m_windowSizeY);
            break;
        case 'R'://reseta o boneco pra pos inicial e rotação inicial
        case 'r':
            m_person.resetPersonPos();
            break;
        case '1'://muda o tipo de movimentação para 'andando'
            m_person.setMovimentType(MovementType::WALKING);
            break;
        case '2'://muda o tipo de movimentação para 'correndo'
            m_person.setMovimentType(MovementType::RUNNING);
            break;
        case '3'://usa a função de 'displacement' do pdf
            m_person.setUseDisplacementFunc(true);
            break;
        case '4'://usa a matriz de 'displacement' criada por mim [gambiarra]
            m_person.setUseDisplacementFunc(false);
            break;
	}

	glutPostRedisplay();
}

void Scene::keyboardSpecialDown(int key, int x, int y){
    KEYS[key] = true;
    switch(key) {
		case GLUT_KEY_F1:{//seta a window para fullscreen
			glutFullScreen();
			setWindowSize(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			break;
		}case GLUT_KEY_F2://seta a window para o tamanho 800x480
			glutReshapeWindow(800, 480);
			break;
        case GLUT_KEY_F3:{//posiciona a camera para a 'visão geral'
            m_viewPos = Vector3(0.0, -2.5, 0.0);
            reshape(m_windowSizeX, m_windowSizeY);
            glutPostRedisplay();
            break;
        }case GLUT_KEY_F4:{//posiciona a camera para a 'visão do chão'
            m_viewPos = Vector3(0.0, -0.009, 0.0);
            reshape(m_windowSizeX, m_windowSizeY);
            glutPostRedisplay();
            break;
        }
	}
}

void Scene::keyboardSpecialUp(int key, int x, int y){
    KEYS[key] = false;
}

void Scene::drawPlane(){
    int color = 1, n = 10;
    glPushMatrix();
        for(int i = -n; i<=n; i += 1){
            for(int j = -n; j<=n; j += 1){
                if(color == 1){
                    glColor3ub(255, 255, 255);
                    color = 2;
                }else{
                    glColor3ub(0, 0, 0);
                    color = 1;
                }
                glBegin(GL_QUADS);
                    glVertex3d(j,   0, i+1);
                    glVertex3d(j,   0, i);
                    glVertex3d(j+1, 0, i);
                    glVertex3d(j+1, 0, i+1);
                glEnd();
            }
        }
    glPopMatrix();
}

