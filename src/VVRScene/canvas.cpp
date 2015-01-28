#include "canvas.h"

#include <vector>
#include <cmath>
#include <QtOpenGL>

#define DEF_LINE_WIDTH 1.5

using namespace vvr;
using namespace std;

/* Commont Color Definitions */
ColRGB ColRGB::white  (0xFF, 0xFF, 0xFF);
ColRGB ColRGB::red    (0xFF, 0x00, 0x00);
ColRGB ColRGB::green  (0x00, 0xFF, 0x00);
ColRGB ColRGB::blue   (0x00, 0x00, 0xFF);
ColRGB ColRGB::black  (0x00, 0x00, 0x00);
ColRGB ColRGB::yellow (0xFF, 0xFF, 0x00);

/* Shape drawing */
void Point2D::drawShape() {
    glPointSize(7);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
}

void LineSeg2D::drawShape() {
    glLineWidth(DEF_LINE_WIDTH);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void Line2D::drawShape() {
    double dx = x2-x1;
    double dy = y2-y1;

    glLineWidth(DEF_LINE_WIDTH);
    glBegin(GL_LINES);
    glVertex2f(x1 - 1000*dx, y1 - 1000*dy);
    glVertex2f(x2 + 1000*dx, y2 + 1000*dy);
    glEnd();
}

void LineSeg3D::drawShape() {
    double dx = x2-x1;
    double dy = y2-y1;

    glLineWidth(DEF_LINE_WIDTH);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glEnd();
}

void Circle2D::drawShape() {
    double x_, y_, theta;
    unsigned const numOfSegments = 30;

    glLineWidth(DEF_LINE_WIDTH);
    glBegin(GL_LINE_LOOP);
    for(int i = 0; i < numOfSegments; i++) {
        theta = 2.0f * 3.14159265 * i / numOfSegments;
        x_ = r * cosf(theta);
        y_ = r * sinf(theta);
        glVertex2f(x + x_, y + y_);
    }
    glEnd();
}

void Triangle2D::drawShape() {
    glLineWidth(DEF_LINE_WIDTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

/* Canvas */
Canvas2D::Canvas2D() {
    frames.push_back(Frame(false));
    fi=0;
}

void Canvas2D::add(Shape *shape_ptr) {
    frames[fi].shapes.push_back(shape_ptr);
}

void Canvas2D::newFrame(bool show_old_frames) {
    frames.push_back(Frame(show_old_frames));
    fi++;
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
    frames.resize(i);
    fi=i-1;
}
