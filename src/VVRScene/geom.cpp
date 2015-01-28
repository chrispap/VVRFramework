#include "geom.h"
#include "canvas.h"

#include <iostream>
#include <cfloat>
#include <vector>
#include <list>
#include <cmath>
#include <cstdlib>
#include <QtOpenGL>

using namespace vvr;
using namespace std;

static const double PI = 3.14159f;

void Vec3d::print() {
    cout << "(" << x << ", " << y << ", " << z << ")" << endl;
}

bool Vec3d::operator== (const Vec3d &p)
{
    return (x == p.x && y == p.y && z == p.z);
}

bool Vec3d::operator!= (const Vec3d &p) 
{
    return !(*this == p);
}

double Vec3d::length() 
{
    return sqrt(x*x + y*y + z*z);
}

Vec3d& Vec3d::normalize() 
{
    scale(1.0f / length());
    return *this;
}

Vec3d& Vec3d::add(const Vec3d &v) 
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vec3d& Vec3d::sub(const Vec3d &v) 
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vec3d& Vec3d::scale(const double s) 
{
    x *= s;
    y *= s;
    z *= s;
    return *this;
}

Box::Box() :
    min(0, 0, 0), 
    max(0, 0, 0)
{

}

Box::Box(const Vec3d &vmin, const Vec3d &vmax) :
min(vmin), 
    max(vmax)
{

}

Box::Box(const Vec3d &v1, const Vec3d &v2, const Vec3d &v3) 
{
    min = Vec3d(std::min(v1.x, std::min(v2.x, v3.x)),
        std::min(v1.y, std::min(v2.y, v3.y)),
        std::min(v1.z, std::min(v2.z, v3.z)));

    max = Vec3d(std::max(v1.x, std::max(v2.x, v3.x)),
        std::max(v1.y, std::max(v2.y, v3.y)),
        std::max(v1.z, std::max(v2.z, v3.z)));
}

Box::Box(const vector<Vec3d> &vertices) 
{
    Vec3d min, max;
    min.x = min.y = min.z = FLT_MAX;
    max.x = max.y = max.z = FLT_MIN;
    vector<Vec3d>::const_iterator vi;
    for (vi = vertices.begin(); vi != vertices.end(); ++vi) {
        if (vi->x > max.x) max.x = vi->x;
        else if (vi->x < min.x) min.x = vi->x;
        if (vi->y > max.y) max.y = vi->y;
        else if (vi->y < min.y) min.y = vi->y;
        if (vi->z > max.z) max.z = vi->z;
        else if (vi->z < min.z) min.z = vi->z;
    }
    this->min = min;
    this->max = max;
}

Box& Box::cropBox(Box &cropBox) 
{
    if (min.x < cropBox.min.x) min.x = cropBox.min.x;
    if (min.y < cropBox.min.y) min.y = cropBox.min.y;
    if (min.z < cropBox.min.z) min.z = cropBox.min.z;
    if (max.x > cropBox.max.x) max.x = cropBox.max.x;
    if (max.y > cropBox.max.y) max.y = cropBox.max.y;
    if (max.z > cropBox.max.z) max.z = cropBox.max.z;
    return *this;
}

double Box::getXSize() const 
{
    return max.x - min.x;
}

double Box::getYSize() const 
{
    return max.y - min.y;
}

double Box::getZSize() const 
{
    return max.z - min.z;
}

Vec3d Box::getSize() const 
{
    return Vec3d(getXSize(), getYSize(), getZSize());
}

double Box::getMinSize() const 
{
    return std::min(getXSize(), std::min(getYSize(), getZSize()));
}

double Box::getMaxSize() const 
{
    return std::max(getXSize(), std::max(getYSize(), getZSize()));
}

double Box::getVolume() const 
{
    return fabs((max.x - min.x)*(max.y - min.y)*(max.z - min.z));
}

Box& Box::add(const Vec3d &v) 
{
    min.add(v);
    max.add(v);
    return *this;
}

Box& Box::sub(const Vec3d &v) 
{
    min.sub(v);
    max.sub(v);
    return *this;
}

Box& Box::scale(const double s) 
{
    min.scale(s);
    max.scale(s);
    return *this;
}

void Box::draw(const ColRGB &col, unsigned char a) const 
{
    static Vec3d p[8];
    Vec3d *v = p;

    *v++ = Vec3d(min.x, min.y, min.z), //0
    *v++ = Vec3d(min.x, max.y, min.z), //1
    *v++ = Vec3d(min.x, max.y, max.z), //2
    *v++ = Vec3d(min.x, min.y, max.z), //3
    *v++ = Vec3d(max.x, min.y, min.z), //4
    *v++ = Vec3d(max.x, max.y, min.z), //5
    *v++ = Vec3d(max.x, max.y, max.z), //6
    *v++ = Vec3d(max.x, min.y, max.z), //7

    glPolygonMode(GL_FRONT_AND_BACK, a ? GL_FILL : GL_LINE);
    glBegin(GL_QUADS);
    if (a) glColor4ub(col.r, col.g, col.b, a);
    else glColor3ubv(col.data);

    glVertex3dv(p[0].data);
    glVertex3dv(p[1].data);
    glVertex3dv(p[2].data);
    glVertex3dv(p[3].data);

    glVertex3dv(p[1].data);
    glVertex3dv(p[2].data);
    glVertex3dv(p[6].data);
    glVertex3dv(p[5].data);

    glVertex3dv(p[4].data);
    glVertex3dv(p[5].data);
    glVertex3dv(p[6].data);
    glVertex3dv(p[7].data);

    glVertex3dv(p[0].data);
    glVertex3dv(p[4].data);
    glVertex3dv(p[7].data);
    glVertex3dv(p[3].data);

    glVertex3dv(p[2].data);
    glVertex3dv(p[3].data);
    glVertex3dv(p[7].data);
    glVertex3dv(p[6].data);

    glVertex3dv(p[0].data);
    glVertex3dv(p[1].data);
    glVertex3dv(p[5].data);
    glVertex3dv(p[4].data);
    glEnd();
}

void Triangle::update() {
    box = Box(v1(), v2(), v3());
    A = v1().y*(v2().z - v3().z) + v2().y*(v3().z - v1().z) + v3().y*(v1().z - v2().z);
    B = v1().z*(v2().x - v3().x) + v2().z*(v3().x - v1().x) + v3().z*(v1().x - v2().x);
    C = v1().x*(v2().y - v3().y) + v2().x*(v3().y - v1().y) + v3().x*(v1().y - v2().y);
    D = -v1().x*(v2().y*v3().z - v3().y*v2().z) - v2().x*(v3().y*v1().z - v1().y*v3().z) - v3().x*(v1().y*v2().z - v2().y*v1().z);
}

const Vec3d& Triangle::v1() const { return (*vecList)[vi1]; }

const Vec3d& Triangle::v2() const { return (*vecList)[vi2]; }

const Vec3d& Triangle::v3() const { return (*vecList)[vi3]; }

const Box& Triangle::getBox() const { return box; }

const Vec3d Triangle::getNormal() const { return Vec3d(A, B, C).normalize(); }

const Vec3d Triangle::getCenter() const {
    return Vec3d(v1()).add(v2()).add(v3()).scale(1.0f / 3);
}

double Triangle::planeEquation(const Vec3d &r) const {
    return A*r.x + B*r.y + C*r.z + D;
}
