#ifndef VVR_DRAWING_H
#define VVR_DRAWING_H

#include "vvrframework_DLL.h"
#include "utils.h"
#include "macros.h"
#include <MathGeoLib.h>
#include <GeoLib.h>
#include <string>
#include <vector>
#include <cstdlib>

namespace vvr {

    typedef float real_t;

    /*--- [Helpers] -------------------------------------------------------------------*/

    math::AABB aabbFromVertices(const std::vector<math::vec> &vertices);

    /*--- [Colour] --------------------------------------------------------------------*/

    struct vvrframework_API Colour
    {
        Colour()
            : r(0)
            , g(0)
            , b(0)
            , a(255)
        { }

        Colour(int red, int green, int blue)
            : r((unsigned char)red)
            , g((unsigned char)green)
            , b((unsigned char)blue)
            , a(255)
        { }

        Colour(float red, float green, float blue)
            : r(red * 0xFF)
            , g(green * 0xFF)
            , b(blue * 0xFF)
            , a(255)
        { }

        Colour(std::string hex_str)
            : r(strtol(hex_str.substr(0, 2).c_str(), 0, 16))
            , g(strtol(hex_str.substr(2, 2).c_str(), 0, 16))
            , b(strtol(hex_str.substr(4, 2).c_str(), 0, 16))
            , a(255)
        { }

        void lighther()
        {
            r = std::max((int)(1.1 * r), 0xFF);
            g = std::max((int)(1.1 * g), 0xFF);
            b = std::max((int)(1.1 * b), 0xFF);
        }

        void darker()
        {
            r = 0.9 * r;
            g = 0.9 * g;
            b = 0.9 * b;
        }

        union
        {
            struct { unsigned char r, g, b, a; };
            unsigned char data[4];
        };

    };

    /*--- [Interfaces] ----------------------------------------------------------------*/

    struct vvrframework_API Drawable
    {
        virtual ~Drawable() { };
        virtual void draw() const = 0;
        void drawif() { if (visible) draw(); }
        bool isVisible() { return visible; }
        bool setVisibility(bool viz) { visible = viz; return visible; }
        bool toggleVisibility() { visible = !visible; return visible; }
        bool show() { visible = true; return visible; }
        bool hide() { visible = false; return visible; }
        bool visible = true;
    };

    struct vvrframework_API Shape : public Drawable
    {
        Shape() { }
        Shape(const Colour &col, bool filled=false) : colour(col), filled(filled) { }
        virtual ~Shape() { }
        virtual void drawShape() const = 0;
        virtual void setup() { };
        void draw() const override;
        Colour colour;
        bool filled = false;
        static real_t LineWidth;
        static real_t PointSize;
    };

    /*--- [Shapes] 2D -----------------------------------------------------------------*/

    struct vvrframework_API Point2D : public Shape
    {
        real_t x, y;

    protected:
        void drawShape() const override;

    public:
        Point2D() {}

        Point2D(real_t x, real_t y, const Colour &col = Colour()) 
            : x(x)
            , y(y)
            , Shape(col) 
        { }
    };

    struct vvrframework_API LineSeg2D : public Shape
    {
        real_t x1, y1;
        real_t x2, y2;

    protected:

        void drawShape() const override;

    public:
        LineSeg2D() {}
        
        LineSeg2D(real_t x1, real_t y1, real_t x2, real_t y2, const Colour &col = Colour()) 
            : x1(x1)
            , y1(y1)
            , x2(x2)
            , y2(y2)
            , Shape(col) 
        { }

        void set(real_t x1, real_t y1, real_t x2, real_t y2)
        {
            vvr_setmemb(x1);
            vvr_setmemb(y1);
            vvr_setmemb(x2);
            vvr_setmemb(y2);
        }
    };

    struct vvrframework_API Line2D : public LineSeg2D
    {
    protected:
        void drawShape() const override;

    public:
        Line2D() {}
        Line2D(real_t x1, real_t y1, real_t x2, real_t y2, const Colour &col = Colour()) 
            : LineSeg2D(x1, y1, x2, y2, col) 
        { }
    };

    struct vvrframework_API Triangle2D : public Shape
    {
        real_t x1, y1;
        real_t x2, y2;
        real_t x3, y3;

    protected:
        void drawShape() const override;

    public:
        Triangle2D() { filled = false; }
        Triangle2D(real_t x1, real_t y1, real_t x2, real_t y2, real_t x3, real_t y3,
            const Colour &col = Colour()) :
            x1(x1), y1(y1), x2(x2), y2(y2), x3(x3), y3(y3), Shape(col) {
            filled = false;
        }
        void set(real_t x1, real_t y1, real_t x2, real_t y2, real_t x3, real_t y3)
        {
            vvr_setmemb(x1);
            vvr_setmemb(y1);
            vvr_setmemb(x2);
            vvr_setmemb(y2);
            vvr_setmemb(x3);
            vvr_setmemb(y3);
        }
    };

    struct vvrframework_API Circle2D : public Shape
    {
        real_t x, y, r;
        real_t rad_from, rad_to;
        bool closed_loop;

    protected:
        void drawShape() const override;

    public:
        Circle2D() : rad_from(0), rad_to(6.28318530718), closed_loop(true) {}
        Circle2D(real_t x, real_t y, real_t rad, const Colour &col = Colour())
            : Shape(col)
            , x(x)
            , y(y)
            , r(rad)
            , rad_from(0)
            , rad_to(6.28318530718)
            , closed_loop(true)
        { }

        void set(real_t x, real_t y, real_t r)
        {
            vvr_setmemb(x);
            vvr_setmemb(y);
            vvr_setmemb(r);
        }

        void setRange(real_t Rad_from, real_t Rad_to) { rad_from = Rad_from; rad_to = Rad_to; }

        void setClosedLoop(bool Closed_loop) { closed_loop = Closed_loop; }
    };

    /*--- [Shapes] 3D -----------------------------------------------------------------*/

    struct vvrframework_API Point3D : public Shape, public math::vec
    {
        vvr_decl_shape(Point3D, math::vec, false);

        Point3D(real_t x, real_t y, real_t z, const Colour &col = Colour())
            : Shape(col)
            , math::vec(x, y, z) 
        { }

    private:
        void drawShape() const override;
    };

    struct vvrframework_API LineSeg3D : public Shape, public math::LineSegment
    {
        vvr_decl_shape(LineSeg3D, math::LineSegment, false);

        LineSeg3D(real_t x1, real_t y1, real_t z1, real_t x2, real_t y2, real_t z2, 
                  const Colour &col = Colour()) 
            : Shape(col) 
            , LineSegment(math::vec{x1, y1, z1}, math::vec{x2, y2, z2})
        { }

    private:
        void drawShape() const override;
    };

    struct vvrframework_API Sphere3D : public Shape, public math::Sphere
    {
        vvr_decl_shape(Sphere3D, math::Sphere, false);

        Sphere3D(real_t x, real_t y, real_t z, real_t r, const Colour &col = Colour()) 
            : Shape(col) 
            , math::Sphere({x,y,z}, r)
        { }

    private:
        void drawShape() const override;
    };

    struct vvrframework_API Aabb3D : public Shape, public math::AABB
    {
        vvr_decl_shape(Aabb3D, math::AABB, false);

        Aabb3D(real_t xmin, real_t ymin, real_t zmin, 
               real_t xmax, real_t ymax, real_t zmax, const Colour &col = Colour())
            : Shape(col)
            , math::AABB(math::vec{ xmin, ymin, zmin }, math::vec{ xmax, ymax, zmax })
        {
            setup();
        }

        Aabb3D(const std::vector<math::vec> vertices, const Colour &col = Colour())
            : Shape(col)
            , math::AABB(aabbFromVertices(vertices))
        {
            setup();
        }

        void setTransparency(real_t a) 
        { 
            transparency = a; 
        }

        real_t transparency;

    private:
        void drawShape() const override;

        void setup() override
        {
            transparency = 0;
        }
    };

    struct vvrframework_API Obb3D : public Shape, private math::OBB
    {
        Obb3D();
        ~Obb3D();
        Obb3D(const Obb3D&) = delete;
        void set(const math::AABB& aabb, const math::float4x4& transform);
        void drawShape() const override;

    private:
        const size_t num_triverts;
        math::vec *triverts;
        math::vec *trinorms;
        math::vec *norms;
        Point3D *cornerpts;
        Colour col_edge;
    };

    struct vvrframework_API Triangle3D : public Shape, public math::Triangle
    {
        vvr_decl_shape(Triangle3D, math::Triangle, true);

        Triangle3D(real_t x1, real_t y1, real_t z1, real_t x2, real_t y2, real_t z2,
                   real_t x3, real_t y3, real_t z3, const Colour &col = Colour()) 
            : Shape(col, true)
            , math::Triangle(
                math::vec{ x1, y1, z1}, 
                math::vec{ x2, y2, z2 }, 
                math::vec{ x3, y3, z3 })
        {
            setup();
        }

        void setColourPerVertex(const Colour &c1, const Colour &c2, const Colour &c3)
        {
            vertex_col[0] = c1;
            vertex_col[1] = c2;
            vertex_col[2] = c3;
        }

        Colour vertex_col[3];

    private:
        void drawShape() const override;
        
        void setup() override
        {
            setColourPerVertex(colour, colour, colour);
        }
    };

    struct vvrframework_API Ground : public Drawable
    {
        Ground(const real_t W, const real_t D, const real_t B, const real_t T, const Colour &colour);
        void draw() const override;

    private:
        std::vector<Triangle3D> m_floor_tris;
        Colour m_col;
    };

    struct vvrframework_API Axes : Drawable
    {
        Axes(real_t d)
            : x(0, 0, 0, d, 0, 0, Colour(1.0f, 0.0f, 0.0f))
            , y(0, 0, 0, 0, d, 0, Colour(0.0f, 1.0f, 0.0f))
            , z(0, 0, 0, 0, 0, d, Colour(0.0f, 0.0f, 1.0f))
        { }

        virtual void draw() const override
        {
            x.draw();
            y.draw();
            z.draw();
        };

    private:
        LineSeg3D x, y, z;
    };

    /*--- [Canvas] --------------------------------------------------------------------*/

    class vvrframework_API Canvas : public Drawable
    {
        struct Frame
        {
            Frame(bool show_old = true) : show_old(show_old) {}
            std::vector<Drawable*> drvec;
            bool show_old;
        };

        size_t fid;
        bool del_on_clear;
        std::vector<Frame> frames;

    public:
        Canvas();
        ~Canvas();
        void draw() const override;
        Drawable* add(Drawable *drawable_ptr);
        std::vector<Drawable*>& getDrawables(int offs = 0) { return frames[fid + offs].drvec; }
        size_t size() const { return frames.size(); }
        size_t frameIndex() const { return fid; }
        void setDelOnClear(bool del) { del_on_clear = del; }
        bool isAtStart() const { return fid == 0; }
        bool isAtEnd() const { return fid == frames.size() - 1; }
        void newFrame(bool show_old_frames = true);
        void next() { if (fid < frames.size() - 1) fid++; }
        void prev() { if (fid > 0) fid--; }
        void rew() { fid = 0; }
        void ff() { fid = frames.size() - 1; }
        void resize(int i);
        void clear();
        void clearFrame();

        /* Helpers to directly add GeoLib objects to canvas */

        Drawable* add(const C2DPoint &p, const Colour &col = Colour())
        {
            return add(new Point2D(p.x, p.y, col));
        }

        Drawable* add(const C2DPoint &p1, const C2DPoint &p2, const Colour &col = Colour(), bool inf = false)
        {
            if (inf)
                return add(new Line2D(p1.x, p1.y, p2.x, p2.y, col));
            else
                return add(new LineSeg2D(p1.x, p1.y, p2.x, p2.y, col));
        }

        Drawable* add(const C2DLine &line, const Colour &col = Colour(), bool inf_line = false)
        {
            const C2DPoint &p1 = line.GetPointFrom();
            const C2DPoint &p2 = line.GetPointTo();
            return add(p1, p2, col, inf_line);
        }

        Drawable* add(const C2DCircle &circle, const Colour &col = Colour(), bool solid = false)
        {
            Shape * s = new Circle2D(circle.GetCentre().x, circle.GetCentre().y, circle.GetRadius(), col);
            s->filled = solid;
            return add(s);
        }

        Drawable* add(const C2DTriangle &tri, const Colour &col = Colour(), bool solid = false)
        {
            Shape *s = new Triangle2D(
                tri.GetPoint1().x,
                tri.GetPoint1().y,
                tri.GetPoint2().x,
                tri.GetPoint2().y,
                tri.GetPoint3().x,
                tri.GetPoint3().y,
                col);
            s->filled = solid;
            return add(s);
        }
    };

    /*--- [Drawing helpers] -----------------------------------------------------------*/

    vvrframework_API void draw(C2DPointSet &point_set, const Colour &col = Colour());

    vvrframework_API void draw(C2DLineSet &line_set, const Colour &col = Colour());

    vvrframework_API void draw(C2DPolygon &polygon, const Colour &col = Colour(), bool filled = false);

    /*--- [MathGeoLib => vvr Converters] (Deprecated. Keep for legacy code)------------*/

    vvrframework_API Triangle3D math2vvr(const math::Triangle &t, const Colour &col);

    vvrframework_API LineSeg3D math2vvr(const math::LineSegment &l, const Colour &col);

    vvrframework_API LineSeg3D math2vvr(const math::Line &l, const Colour &col);

    vvrframework_API Point3D math2vvr(const math::vec &v, const Colour &col);

    /*--- [Predefined colours] --------------------------------------------------------*/

    extern const vvrframework_API Colour white;
    extern const vvrframework_API Colour red;
    extern const vvrframework_API Colour green;
    extern const vvrframework_API Colour blue;
    extern const vvrframework_API Colour black;
    extern const vvrframework_API Colour yellow;
    extern const vvrframework_API Colour grey;
    extern const vvrframework_API Colour orange;
    extern const vvrframework_API Colour cyan;
    extern const vvrframework_API Colour magenta;
    extern const vvrframework_API Colour darkOrange;
    extern const vvrframework_API Colour darkRed;
    extern const vvrframework_API Colour darkGreen;
    extern const vvrframework_API Colour yellowGreen;
    extern const vvrframework_API Colour lilac;

    /*---------------------------------------------------------------------------------*/
}

#endif
