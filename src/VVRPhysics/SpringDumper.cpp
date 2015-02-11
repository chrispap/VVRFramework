#include "SpringDumper.h"

#include "GL/glut.h"

SpringDumper::SpringDumper(
	Vector3 pos, Vector3 vel, float radius, float mass, 
	Vector3 anchor, float stiffness, float dumping, float restLength)
	: s1(pos, vel, radius, mass), 
	a(anchor), k(stiffness), b(dumping), l0(restLength)
{

}


SpringDumper::~SpringDumper()
{

}

void SpringDumper::draw()
{
	glPushMatrix();

	//draw spring
	glLineWidth(3);
	glBegin(GL_LINES);
		glVertex3f(a.x, a.y, a.z);
		glVertex3f(s1.x.x, s1.x.y, s1.x.z);
	glEnd();
	glLineWidth(1);

	//draw sphere
	s1.draw();

	glPopMatrix();
}


void SpringDumper::update(float t)
{
	//collision

	//forces

	// Task
	s1.f = Vector3(0, 0, 0);
	
	s1.f += s1.m * 9.81 * Vector3(0, -1, 0);

	Vector3 x_ij = a - s1.x;
	s1.f += k * (x_ij.length() - l0) * x_ij.normalize() - b * s1.v;
	

	//integration
	float* new_state = s1.integrate(t, STATES, s1.getState(), dt);
	s1.setState(new_state);

}
