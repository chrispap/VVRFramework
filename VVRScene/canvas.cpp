#include "canvas.h"
#include "geom.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <QtOpenGL>
#include <MathGeoLib.h>

#define DEF_LINE_WIDTH 2.2

using namespace vvr;
using namespace std;

/* Common Color Definitions */
const Colour Colour::red            (0xFF, 0x00, 0x00);
const Colour Colour::blue           (0x00, 0x00, 0xFF);
const Colour Colour::grey           (0x66, 0x66, 0x66);
const Colour Colour::cyan           (0x00, 0xFF, 0xFF);
const Colour Colour::white          (0xFF, 0xFF, 0xFF);
const Colour Colour::green          (0x00, 0xFF, 0x00);
const Colour Colour::black          (0x00, 0x00, 0x00);
const Colour Colour::yellow         (0xFF, 0xFF, 0x00);
const Colour Colour::orange         (0xFF, 0x66, 0x00);
const Colour Colour::magenta        (0xFF, 0x00, 0xFF);
const Colour Colour::darkRed        (0x8B, 0x00, 0x00);
const Colour Colour::darkOrange     (0xFF, 0x8C, 0x00);
const Colour Colour::darkGreen      (0x00, 0x64, 0x00);
const Colour Colour::yellowGreen    (0x9A, 0xCD, 0x32);

/* Shape drawing */
void Shape::draw() const {
    glPolygonMode(GL_FRONT_AND_BACK, b_render_solid ? GL_FILL : GL_LINE);
    glColor3ubv(colour.data);
    drawShape();
}

void Point2D::drawShape() const {
    glPointSize(7);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
}

void Point3D::drawShape() const {
    glPointSize(7);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex3f(x,y,z);
    glEnd();
}

void LineSeg2D::drawShape() const {
    glLineWidth(DEF_LINE_WIDTH);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void Line2D::drawShape() const {
    double dx = x2-x1;
    double dy = y2-y1;

    glLineWidth(DEF_LINE_WIDTH);
    glBegin(GL_LINES);
    glVertex2f(x1 - 1000*dx, y1 - 1000*dy);
    glVertex2f(x2 + 1000*dx, y2 + 1000*dy);
    glEnd();
}

void LineSeg3D::drawShape() const {
    double dx = x2-x1;
    double dy = y2-y1;

    glLineWidth(DEF_LINE_WIDTH);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}

void Circle2D::drawShape() const {
    if (rad_from >= rad_to) {
        std::cerr << "Trying to render circle with [rad_from >= rad_to]" << std::endl;
        return;
    }

    double x_, y_;
    unsigned const numOfSegments = 60;

    glLineWidth(DEF_LINE_WIDTH);
    glBegin(b_render_solid? GL_POLYGON : (closed_loop?GL_LINE_LOOP:GL_LINE_STRIP));
    double d_th = (rad_to - rad_from) / numOfSegments;
    for(double theta = rad_from; theta <= rad_to; theta+=d_th) {
        x_ = r * cosf(theta);
        y_ = r * sinf(theta);
        glVertex2f(x + x_, y + y_);
    }
    glEnd();

}

void Sphere3D::drawShape() const {
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(rad, rad, rad);
    drawSphere(rad, 12, 15);
    glPopMatrix();
}

void Box3D::drawShape() const {
    Box box(Vec3d(x1, y1, z1), Vec3d(x2, y2, z2));
    box.draw(colour, 0x100);
    box.draw(colour, 0);
}

void Triangle2D::drawShape() const {
    glLineWidth(DEF_LINE_WIDTH);
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void Triangle3D::drawShape() const {
    glLineWidth(DEF_LINE_WIDTH);
    glBegin(GL_TRIANGLES);
    math::vec n = math::Triangle(
        math::vec(x1,y1,z1),
        math::vec(x2,y2,z2),
        math::vec(x3,y3,z3)
    ).NormalCW();
    
    glNormal3fv(n.ptr());

    glColor3ubv(vertex_col[0].data);
    glVertex3f(x1, y1, z1);
    
    glColor3ubv(vertex_col[1].data);
    glVertex3f(x2, y2, z2);

    glColor3ubv(vertex_col[2].data);
    glVertex3f(x3, y3, z3);

    glEnd();
}

/* Canvas */
Frame::Frame() : show_old(true)
{

}

Frame::Frame(bool show_old) : show_old(show_old)
{

}

Canvas2D::Canvas2D()
{
    clear();
}

Canvas2D::~Canvas2D()
{
    for (int fi=0; fi<frames.size(); fi++) {
        for (int si=0; si<frames[fi].shapes.size(); si++) {
            delete frames[fi].shapes[si];
        }
    }
}

void Canvas2D::add(Shape *shape_ptr) {
    frames[fi].shapes.push_back(shape_ptr);
}

void Canvas2D::newFrame(bool show_old_frames) {
    frames.push_back(Frame(show_old_frames));
    ff();
}

void Canvas2D::draw() {
    Frame *frame;
    int fi_ = (int) fi;
    while (frames[fi_].show_old && --fi_>=0);
    while(fi_ <= fi) {
        frame = &frames[fi_];
        for (unsigned i=0; i<frames[fi_].shapes.size(); i++)
            frames[fi_].shapes[i]->draw();
        fi_++;
    }

}

void Canvas2D::next() {
    if (fi<frames.size()-1) fi++;
}

void Canvas2D::prev() {
    if (fi>0) fi--;
}

void Canvas2D::rew() {
    fi = 0;
}

void Canvas2D::ff() {
    fi = frames.size()-1;
}

void Canvas2D::resize(int i) {
    if (i<1 || i > size()-1) return;

    // Delete shapes of frames that will be discarded
    for (int fi=i; fi<frames.size(); fi++) {
        for (int si=0; si<frames[fi].shapes.size(); si++) {
            delete frames[fi].shapes[si];
        }
    }

    frames.resize(i);
    fi=i-1;
}

void Canvas2D::clear()
{
    // Delete shapes of frames that will be discarded
    for (int fi=0; fi<frames.size(); fi++) {
        for (int si=0; si<frames[fi].shapes.size(); si++) {
            delete frames[fi].shapes[si];
        }
    }

    frames.clear();
    frames.push_back(Frame(false));
    fi=0;
}

void drawSphere(double r, int lats, int longs)
{
    int i, j;
    for (i = 0; i <= lats; i++) {
        double lat0 = M_PI * (-0.5 + (double)(i - 1) / lats);
        double z0 = sin(lat0);
        double zr0 = cos(lat0);

        double lat1 = M_PI * (-0.5 + (double)i / lats);
        double z1 = sin(lat1);
        double zr1 = cos(lat1);
        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= longs; j++) {
            double lng = 2 * M_PI * (double)(j - 1) / longs;
            double x = cos(lng);
            double y = sin(lng);
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
}

void vvr::draw(C2DPointSet &point_set, const Colour &col)
{
    /* Draw point cloud */
    for (int i = 0; i < point_set.size(); i++) {
        Point2D(
            point_set.GetAt(i)->x,
            point_set.GetAt(i)->y,
            col).draw();
    }
}

void vvr::draw(C2DLineSet  &line_set, const Colour &col)
{
    for (int i = 0; i < line_set.size(); i++) {
        LineSeg2D(
            line_set.GetAt(i)->GetPointFrom().x,
            line_set.GetAt(i)->GetPointFrom().y,
            line_set.GetAt(i)->GetPointTo().x,
            line_set.GetAt(i)->GetPointTo().y,
            col).draw();
    }
}

void vvr::draw(C2DPolygon  &polygon, const Colour &col, bool filled)
{
    if (filled)
    {
        C2DPolygonSet polygon_set;
        if (!polygon.IsConvex())
            polygon.CreateConvexSubAreas();
        polygon.GetConvexSubAreas(polygon_set);
        for (int i=0; i<polygon_set.size(); i++) {
            C2DPolygon &pol_conv = *polygon_set.GetAt(i);
            C2DPoint pol_conv_centroid = pol_conv.GetCentroid();
            for (int j = 0; j < pol_conv.GetLines().size(); j++) {
                Triangle2D t(
                            pol_conv.GetLines().GetAt(j)->GetPointFrom().x,
                            pol_conv.GetLines().GetAt(j)->GetPointFrom().y,
                            pol_conv.GetLines().GetAt(j)->GetPointTo().x,
                            pol_conv.GetLines().GetAt(j)->GetPointTo().y,
                            pol_conv_centroid.x, pol_conv_centroid.y);
                t.setSolidRender(true);
                t.setColour(col);
                t.draw();
            }
        }
    }
    else
    {
        for (int i = 0; i < polygon.GetLines().size(); i++) {
            LineSeg2D(
                        polygon.GetLines().GetAt(i)->GetPointFrom().x,
                        polygon.GetLines().GetAt(i)->GetPointFrom().y,
                        polygon.GetLines().GetAt(i)->GetPointTo().x,
                        polygon.GetLines().GetAt(i)->GetPointTo().y,
                        col).draw();
        }
    }
}
