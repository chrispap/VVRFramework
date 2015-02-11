#include <iostream>

#include "Box.h"
#include "GL/glut.h"

using namespace std;
using namespace vvr::phys;

Box::Box(float s)
{
	size = s;
}


Box::~Box()
{
	
}

void Box::draw()
{

	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();

		glLineWidth(2);
		glutWireCube(size);
		glLineWidth(1);

	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopAttrib();

}

float Box::getSize()
{
   return size;
}
