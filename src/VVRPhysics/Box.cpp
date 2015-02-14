#include "Box.h"
#include <iostream>
#include <GL/glut.h>

using namespace vvr::phys;

Box::Box(float s)
{
    size = s;
}

void Box::draw() const
{
    glLineWidth(2);
    glutWireCube(size);
    glLineWidth(1);
}

float Box::getSize()
{
    return size;
}
