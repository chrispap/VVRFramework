#ifndef __SHAPE2D_H__
#define __SHAPE2D_H__

#include "vvrscenedll.h"

#include <string>
#include <vector>
#include <cstdlib>

using std::string;
using std::vector;

namespace vvr {

struct VVRScene_API Colour
{
    union
    {
        struct { unsigned char r, g, b;};
        unsigned char data[3];
    };

    Colour () : r(0), g(0), b(0) {}

    Colour (unsigned char red, unsigned char green, unsigned char blue) :
        r(red), g(green), b(blue) {}

    Colour(string hex_str) {
        r = strtol(hex_str.substr(0,2).c_str(), 0, 16);
        g = strtol(hex_str.substr(2,2).c_str(), 0, 16);
        b = strtol(hex_str.substr(4,2).c_str(), 0, 16);
    }

    static Colour white;
    static Colour red;
    static Colour green;
    static Colour blue;
    static Colour black;
    static Colour yellow;
    static Colour grey;
};

/* Shapes */
struct VVRScene_API Shape
{
    Colour colour;

protected:
    Shape(){}
    Shape(const Colour &rgb) : colour(rgb) {}
    virtual void drawShape() = 0;

public:
    virtual ~Shape(){}
    void draw();
};

struct VVRScene_API Point2D : public Shape
{
    double x,y;

protected:
    void drawShape();

public:
    Point2D(){}
    Point2D(double _x, double _y, const Colour &rgb=Colour()) :
      x(_x), y(_y), Shape(rgb) {}
};

struct VVRScene_API LineSeg2D : public Shape
{
protected:
    double x1,y1;
    double x2,y2;

    void drawShape();

public:
    LineSeg2D(){}
    LineSeg2D(double _x1, double _y1, double _x2, double _y2, const Colour &rgb=Colour()) :
      x1(_x1), y1(_y1), x2(_x2), y2(_y2), Shape(rgb) {}
};

struct VVRScene_API Line2D : public LineSeg2D
{
protected:
    void drawShape();

public:
    Line2D(){}
    Line2D(double _x1, double _y1, double _x2, double _y2, const Colour &rgb=Colour()) :
      LineSeg2D(_x1, _y1, _x2, _y2, rgb) {}
};

struct VVRScene_API LineSeg3D : public Shape
{
protected:
    double x1,y1,z1;
    double x2,y2,z2;

    void drawShape();

public:
    LineSeg3D(){}
    LineSeg3D(double _x1, double _y1, double _z1,
              double _x2, double _y2, double _z2, const Colour &rgb=Colour()) :
      x1(_x1), y1(_y1), z1(_z1), x2(_x2), y2(_y2), z2(_z2), Shape(rgb) {}
};

struct VVRScene_API Circle2D : public Shape
{
    double x,y,r;

protected:
    void drawShape();

public:
    Circle2D(){}
    Circle2D(double cx, double cy, double rad, const Colour &rgb=Colour()) :
      x(cx), y(cy), r(rad), Shape(rgb) {}
};

struct VVRScene_API Triangle2D : public Shape
{
    double x1,y1;
    double x2,y2;
    double x3,y3;

    void drawShape();

public:
    Triangle2D(){}
    Triangle2D(double _x1, double _y1, double _x2, double _y2, double _x3, double _y3, 
        const Colour &rgb=Colour()) :
      x1(_x1), y1(_y1), x2(_x2), y2(_y2), x3(_x3), y3(_y3), Shape(rgb) {}
};

/* Canvas */
struct VVRScene_API Frame {
    vector<Shape*> shapes;
    bool show_old;
    Frame ();
    Frame (bool show_old);
};

class VVRScene_API Canvas2D {
    vector<Frame> frames;
    unsigned fi;

public:
    Canvas2D();
    ~Canvas2D();

    unsigned size() { return frames.size(); }
    unsigned frameIndex() { return fi; }
    bool isAtStart() { return fi == 0; }
    bool isAtEnd() { return fi == frames.size()-1; }

    void newFrame(bool show_old_frames=true);
    void add(Shape *shape_ptr);
    void draw();
    void next();
    void prev();
    void rew();
    void ff();
    void resize(int i);
    void clear();
};

}

#endif
