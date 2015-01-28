#ifndef __SHAPE2D_H__
#define __SHAPE2D_H__

#include "vvrscenedll.h"

#include <string>
#include <vector>

using namespace std;

namespace vvr {

struct vvrscene_API ColRGB
{
    union
    {
        struct { unsigned char r, g, b;};
        unsigned char data[3];
    };

    ColRGB () : r(0), g(0), b(0) {}

    ColRGB (unsigned char red, unsigned char green, unsigned char blue) :
        r(red), g(green), b(blue) {}

    ColRGB(string hex_str) {
        r = strtol(hex_str.substr(0,2).c_str(), 0, 16);
        g = strtol(hex_str.substr(2,2).c_str(), 0, 16);
        b = strtol(hex_str.substr(4,2).c_str(), 0, 16);
    }

    static ColRGB white;
    static ColRGB red;
    static ColRGB green;
    static ColRGB blue;
    static ColRGB black;
    static ColRGB yellow;
};

/* Shapes */
class vvrscene_API Shape
{
    ColRGB colour;

protected:
    Shape(const ColRGB &rgb) : colour(rgb) {}
    virtual void drawShape() = 0;

public:
    void draw();
};

class vvrscene_API Point2D : public Shape
{
    double x,y;

protected:
    void drawShape();

public:
    Point2D(double _x, double _y, const ColRGB &rgb) :
      x(_x), y(_y), Shape(rgb) {}

};

class vvrscene_API LineSeg2D : public Shape
{
protected:
    double x1,y1;
    double x2,y2;

    void drawShape();

public:
    LineSeg2D(double _x1, double _y1, double _x2, double _y2, const ColRGB &rgb) :
      x1(_x1), y1(_y1), x2(_x2), y2(_y2), Shape(rgb) {}

};

class vvrscene_API Line2D : public LineSeg2D
{
protected:
    void drawShape();

public:
    Line2D(double _x1, double _y1, double _x2, double _y2, const ColRGB &rgb) :
      LineSeg2D(_x1, _y1, _x2, _y2, rgb) {}

};

class vvrscene_API LineSeg3D : public Shape
{
protected:
    double x1,y1,z1;
    double x2,y2,z2;

    void drawShape();

public:
    LineSeg3D(double _x1, double _y1, double _z1,
              double _x2, double _y2, double _z2, const ColRGB &rgb) :
      x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2), z2(_z2), Shape(rgb) {}

};

class vvrscene_API Circle2D : public Shape
{
    double x,y,r;

protected:
    void drawShape();

public:
    Circle2D(double cx, double cy, double rad, const ColRGB &rgb) :
      x(cx), y(cy), r(rad), Shape(rgb) {}
};

class vvrscene_API Triangle2D : public Shape
{
    double x1,y1;
    double x2,y2;
    double x3,y3;

    void drawShape();

public:
    Triangle2D(double _x1, double _y1, double _x2, double _y2, double _x3, double _y3, const ColRGB &rgb) :
      x1(_x1), y1(_y1), x2(_x2), y2(_y2), x3(_x3), y3(_y3), Shape(rgb) {}

};

/* Canvas */
struct vvrscene_API Frame {
    vector<Shape*> shapes;
    bool show_old;

    Frame(bool _show_old=true) :
    show_old(_show_old) {};
};

class vvrscene_API Canvas2D {
    vector<Frame> frames;
    unsigned fi;

public:
    Canvas2D();

    unsigned size()       { return frames.size(); }
    unsigned frameIndex() { return fi; }
    bool isAtStart()      { return fi == 0; }
    bool isAtEnd()        { return fi == frames.size()-1; }

    void newFrame(bool show_old_frames=true);
    void add(Shape *shape_ptr);
    void draw();
    void next();
    void prev();
    void rew();
    void ff();
    void resize(int i);

};

}

#endif
