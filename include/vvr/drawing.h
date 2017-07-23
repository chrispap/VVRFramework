#ifndef VVR_CANVAS_H
#define VVR_CANVAS_H

#include "vvrframework_DLL.h"
#include "utils.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <GeoLib.h>
#include <MathGeoLib.h>

namespace vvr {

struct vvrframework_API Colour
{
    union
    {
        struct { unsigned char r, g, b;};
        unsigned char data[3];
    };

    Colour () : r(0), g(0), b(0) {}

    Colour (unsigned char red, unsigned char green, unsigned char blue) :
        r(red), g(green), b(blue) {}

    Colour(std::string hex_str) {
        r = strtol(hex_str.substr(0,2).c_str(), 0, 16);
        g = strtol(hex_str.substr(2,2).c_str(), 0, 16);
        b = strtol(hex_str.substr(4,2).c_str(), 0, 16);
    }

    static const Colour white;
    static const Colour red;
    static const Colour green;
    static const Colour blue;
    static const Colour black;
    static const Colour yellow;
    static const Colour grey;
    static const Colour orange;
    static const Colour cyan;
    static const Colour magenta;
    static const Colour darkOrange;
    static const Colour darkRed;
    static const Colour darkGreen;
    static const Colour yellowGreen;
};

struct vvrframework_API Drawable 
{
    virtual ~Drawable() = default;
    virtual void draw() const = 0;
    bool isVisible() { return visible; }
    bool setVisibility(bool viz) { visible = viz; return visible; }
    bool toggleVisibility() { visible = !visible; return visible; }
    bool show() { visible = true; return visible; }
    bool hide() { visible = false; return visible; }

private:
    bool visible = true;
};

class vvrframework_API Shape : public Drawable
{
protected:
    Colour colour;
    bool render_solid;
    Shape(const Colour &rgb=Colour()) : colour(rgb), render_solid(false) {}
    virtual void drawShape() const = 0;

public:
    virtual ~Shape() {}
    void draw() const override;
    void setColour(const Colour &col) {colour = col;}
    void setRenderSolid(bool solid) {render_solid = solid;}

public:
    static float LineWidth;
    static float PointSize;
};

struct vvrframework_API Point2D : public Shape
{
    double x,y;

protected:
    void drawShape() const override;

public:
    Point2D(){}
    Point2D(double x, double y, const Colour &rgb=Colour()) :
        x(x), y(y), Shape(rgb) {}
};

struct vvrframework_API Point3D : public Shape
{
    double x,y,z;

protected:
    void drawShape() const override;

public:
    Point3D(){}
    Point3D(double x, double y, double z, const Colour &rgb=Colour()) :
        x(x), y(y), z(z), Shape(rgb) {}
};

struct vvrframework_API LineSeg2D : public Shape
{
    double x1,y1;
    double x2,y2;

protected:

    void drawShape() const override;

public:
    LineSeg2D(){}
    LineSeg2D(double _x1, double _y1, double _x2, double _y2, const Colour &rgb=Colour()) :
        x1(_x1), y1(_y1), x2(_x2), y2(_y2), Shape(rgb) {}
};

struct vvrframework_API Line2D : public LineSeg2D
{
protected:
    void drawShape() const override;

public:
    Line2D(){}
    Line2D(double _x1, double _y1, double _x2, double _y2, const Colour &rgb=Colour()) :
        LineSeg2D(_x1, _y1, _x2, _y2, rgb) {}
};

struct vvrframework_API LineSeg3D : public Shape
{
    double x1,y1,z1;
    double x2,y2,z2;

protected:

    void drawShape() const override;

public:
    LineSeg3D(){}
    LineSeg3D(double x1, double y1, double z1,
              double x2, double y2, double z2, const Colour &rgb=Colour()) :
        x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2), Shape(rgb) {}
};

struct vvrframework_API Circle2D : public Shape
{
    double x,y,r;
    double rad_from, rad_to;
    bool closed_loop;
protected:
    void drawShape() const override;

public:
    Circle2D() : rad_from(0), rad_to(6.28318530718), closed_loop(true) {}
    Circle2D(double x, double y, double rad, const Colour &rgb=Colour()) : Shape(rgb),
        x(x), y(y), r(rad), rad_from(0), rad_to(6.28318530718), closed_loop(true) {}
    void setRange(double Rad_from, double Rad_to) {rad_from = Rad_from;rad_to = Rad_to;}
    void setClosedLoop(bool Closed_loop) {closed_loop = Closed_loop;}
};

struct vvrframework_API Sphere3D : public Shape
{
    double x, y, z, rad;

protected:
    void drawShape() const override;

public:
    Sphere3D(){}
    Sphere3D(double x, double y, double z, double rad, const Colour &rgb = Colour()) :
        x(x), y(y), z(z), rad(rad), Shape(rgb) {}
};

struct vvrframework_API Aabb3D : public Shape
{
    double x1, y1, z1;
    double x2, y2, z2;
    float transparency;

protected:
    void drawShape() const override;

public:
    Aabb3D() : transparency(0) {}

    Aabb3D(const std::vector<vec> vertices, const Colour &col = Colour());

    Aabb3D(double xmin, double ymin, double zmin,
        double xmax, double ymax, double zmax,
        const Colour &col = Colour())
        : x1(xmin), y1(ymin), z1(zmin)
        , x2(xmax), y2(ymax), z2(zmax)
        , Shape(col), transparency(0) {}


    void setTransparency(float a) { transparency = a; }
};

struct vvrframework_API Obb3D : public Shape, private math::OBB
{
    Obb3D();
    ~Obb3D();
    Obb3D(const Obb3D&) = delete;
    void set(const math::AABB& aabb, const float4x4& transform);
    void drawShape() const override;

private:
    vvr::Point3D *cornerpts;
    vec *verts;
    vec *norms;
    const size_t num_verts;
    vvr::Colour col_edge;
};

struct vvrframework_API Triangle2D : public Shape
{
    double x1,y1;
    double x2,y2;
    double x3,y3;

protected:
    void drawShape() const override;

public:
    Triangle2D(){render_solid = false;}
    Triangle2D(double x1, double y1, double x2, double y2, double x3, double y3,
               const Colour &rgb=Colour()) :
        x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3), Shape(rgb) {render_solid = false;}
};

struct vvrframework_API Triangle3D : public Shape
{
    double x1,y1,z1;
    double x2,y2,z2;
    double x3,y3,z3;
    Colour vertex_col[3]; // vertex colour

protected:
    void drawShape() const override;

public:
    Triangle3D()
    {
        render_solid = true; 
        vertex_col[0] = colour;
        vertex_col[1] = colour;
        vertex_col[2] = colour;
    }

    Triangle3D(double x1, double y1, double z1,
               double x2, double y2, double z2,
               double x3, double y3, double z3,
               const Colour &rgb=Colour()) :
        x1(x1), y1(y1), z1(z1),
        x2(x2), y2(y2), z2(z2),
        x3(x3), y3(y3), z3(z3),
        Shape(rgb) 
    {
        render_solid = true;
        vertex_col[0] = colour;
        vertex_col[1] = colour;
        vertex_col[2] = colour;
    }

    void setColourPerVertex(const Colour &c1, const Colour &c2, const Colour &c3) 
    {
        vertex_col[0] = c1; vertex_col[1] = c2; vertex_col[2] = c3;
    }
};

struct vvrframework_API Ground : public vvr::Drawable
{
    Ground(const float W, const float D, const float B, const float T, const vvr::Colour &colour);
    void draw() const override;

private:
    std::vector<math::Triangle> m_floor_tris;
    vvr::Colour m_col;
};

struct vvrframework_API GlobalAxes : vvr::Drawable
{
    GlobalAxes(double d)
        : x(0, 0, 0, d, 0, 0, vvr::Colour::red)
        , y(0, 0, 0, 0, d, 0, vvr::Colour::green)
        , z(0, 0, 0, 0, 0, d, vvr::Colour::blue)
    {
    }

    virtual void draw() const override
    {
        x.draw();
        y.draw();
        z.draw();
    };

private:
    vvr::LineSeg3D x, y, z;
};

struct vvrframework_API Frame 
{
    std::vector<Drawable*> drawables;
    bool show_old;
    Frame(bool show_old = true) : show_old(show_old) {}
};

class vvrframework_API Canvas 
{
    unsigned fid;
    std::vector<Frame> frames;

public:
    Canvas();
    ~Canvas();
    unsigned size() { return frames.size(); }
    unsigned frameIndex() { return fid; }
    bool isAtStart() { return fid == 0; }
    bool isAtEnd() { return fid == frames.size()-1; }
    std::vector<Drawable*>& getDrawables(int offs=0) { return frames[fid+offs].drawables; }
    void newFrame(bool show_old_frames=true);
    Drawable* add(Drawable *drawable_ptr);
    void draw();
    void next();
    void prev();
    void rew();
    void ff();
    void resize(int i);
    void clear();
    void clearFrame();

    /* Helpers to directly add GeoLib objects to canvas */

    Drawable* add(const C2DPoint &p, const Colour &col=Colour::black) 
    {
        return add(new Point2D(p.x, p.y, col));
    }

    Drawable* add(const C2DPoint &p1, const C2DPoint &p2, const Colour &col=Colour::black, bool inf_line=false) 
    {
        if (inf_line)
            return add(new Line2D(p1.x, p1.y, p2.x, p2.y, col));
        else
            return add(new LineSeg2D(p1.x, p1.y, p2.x, p2.y, col));
    }

    Drawable* add(const C2DLine &line, const Colour &col=Colour::black, bool inf_line=false) 
    {
        const C2DPoint &p1 = line.GetPointFrom();
        const C2DPoint &p2 = line.GetPointTo();
        return add(p1, p2, col, inf_line);
    }

    Drawable* add(const C2DCircle &circle, const Colour &col=Colour::black, bool solid=false) 
    {
        Shape * s = new Circle2D(circle.GetCentre().x, circle.GetCentre().y, circle.GetRadius(), col);
        s->setRenderSolid(solid);
        return add(s);
    }

    Drawable* add(const C2DTriangle &tri, const Colour &col=Colour::black, bool solid=false) 
    {
        Shape *s = new Triangle2D(
                    tri.GetPoint1().x,
                    tri.GetPoint1().y,
                    tri.GetPoint2().x,
                    tri.GetPoint2().y,
                    tri.GetPoint3().x,
                    tri.GetPoint3().y,
                    col);
        s->setRenderSolid(solid);
        return add(s);
    }

};

vvrframework_API void draw(C2DPointSet &point_set, const Colour &col = Colour::black);

vvrframework_API void draw(C2DLineSet &line_set, const Colour &col = Colour::black);

vvrframework_API void draw(C2DPolygon &polygon, const Colour &col = Colour::black, bool filled = false);

vvrframework_API vvr::Triangle3D math2vvr(const math::Triangle &t, const vvr::Colour &col);

vvrframework_API vvr::LineSeg3D math2vvr(const math::LineSegment &l, const vvr::Colour &col);

vvrframework_API vvr::LineSeg3D math2vvr(const math::Line &l, const vvr::Colour &col);

vvrframework_API vvr::Point3D math2vvr(const math::vec &v, const vvr::Colour &col);

}

#endif
