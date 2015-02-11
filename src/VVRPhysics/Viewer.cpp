#include "Viewer.h"
#include "GL/glut.h"

Viewer *Viewer::instance = NULL;

Viewer::Viewer(int argc, char** argv) 
{
	instance = this;
	t = 0;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize (WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition (WINDOW_X_POS, WINDOW_Y_POS);
	glutCreateWindow (WINDOW_NAME);
	init ();
	glutDisplayFunc(setDisplayFunction);
	glutIdleFunc(setIdeleFunction);
	glutReshapeFunc(setReshapeFunction);
}

Viewer::~Viewer() 
{

	for(unsigned i = 0;i<drawable.size();i++){
		delete drawable.at(i);
	}

}

void Viewer::addToDraw(IRenderable *r)
{
	drawable.push_back(r);	
}

void Viewer::start()
{
	glutMainLoop();
}

void Viewer::init()
{
	//shader
	glShadeModel (GL_SMOOTH);

	//light
	GLfloat light_position[] = {L_POS_X, L_POS_Y, L_POS_Z, L_POS_W};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	GLfloat ambient[] = {.1, .1, .1, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	GLfloat diffuse[] = {.3, .3, .3, 1.0};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	GLfloat specular[] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	GLfloat ratio = (GLfloat) WINDOW_WIDTH/(GLfloat) WINDOW_HEIGHT;
	gluPerspective(40.0, ratio, 1.0f, 200.0f);
	glMatrixMode (GL_MODELVIEW);
}

void Viewer::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	gluLookAt(
		CAM_POS_X, CAM_POS_Y, CAM_POS_Z,
		CAM_VIEW_X, CAM_VIEW_Y, CAM_VIEW_Z,
		CAM_UP_X, CAM_UP_Y, CAM_UP_Z);

	for(unsigned i = 0;i<drawable.size();i++){
		drawable.at(i)->draw();
	}
		
	glutSwapBuffers();
}

void Viewer::update()
{
	for(unsigned i = 0;i<drawable.size();i++) {
		drawable[i]->update(t);
	}

	t += dt;

	glutPostRedisplay();
}

void Viewer::reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	GLfloat ratio = (GLfloat) w/(GLfloat) h;
	gluPerspective(40.0, ratio, 1.0f, 200.0f);
	glMatrixMode (GL_MODELVIEW);
}

void Viewer::setDisplayFunction()
{
	instance->render();
}

void Viewer::setIdeleFunction()
{
	instance->update();
}

void Viewer::setReshapeFunction(int w, int h)
{
	instance->reshape(w, h);
}
