#ifndef VVR_DRAWING_H
#define VVR_DRAWING_H

#include "vvrframework_DLL.h"
#include "utils.h"
#include "macros.h"
#include <MathGeoLib.h>
#include <GeoLib.h>
#include <string>
#include <vector>
#include <array>
#include <cstdlib>

namespace vvr {

    typedef float real;
    class Canvas;

    /*---[Helpers]----------------------------------------------------------------------*/

    math::AABB aabbFromVertices(const std::vector<math::vec> &vertices);

    /*---[Colour]-----------------------------------------------------------------------*/

    struct vvrframework_API Colour
    {
        Colour()
            : r(0)
            , g(0)
            , b(0)
            , a(255)
        { }

        Colour(int r, int g, int b)
            : r((unsigned char)r)
            , g((unsigned char)g)
            , b((unsigned char)b)
            , a(255)
        { }

        Colour(float r, float g, float b)
            : r((unsigned char)(r * 0xFF))
            , g((unsigned char)(g * 0xFF))
            , b((unsigned char)(b * 0xFF))
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
            r = std::min((int)(1.2f * r), 0xFF);
            g = std::min((int)(1.2f * g), 0xFF);
            b = std::min((int)(1.2f * b), 0xFF);
        }

        void darker()
        {
            r = (unsigned char)(0.9f * r);
            g = (unsigned char)(0.9f * g);
            b = (unsigned char)(0.9f * b);
        }

        void mul(float c)
        {
            r = std::min((int)(c * r), 0xFF);
            g = std::min((int)(c * g), 0xFF);
            b = std::min((int)(c * b), 0xFF);
        }

        union
        {
            struct { unsigned char r, g, b, a; };
            unsigned char data[4];
        };

    };

    /*---[Interfaces]-------------------------------------------------------------------*/

    struct vvrframework_API Drawable
    {
        virtual ~Drawable() { }
        virtual void draw() const = 0;
        virtual real pickdist(int x, int y) const { return -1.0f; }
        virtual real pickdist(const math::Ray&) const { return -1.0f; }
        virtual void addToCanvas(Canvas &canvas);
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
        Shape(Colour col, bool filled=false) : colour(col), filled(filled) { }
        virtual ~Shape() { }
        virtual void drawShape() const = 0;
        virtual void setup() { }
        void draw() const override;
        Colour colour;
        bool filled = false;
        static real LineWidth;
        static real PointSize;
    };

    /*---[Shapes: 2D]-------------------------------------------------------------------*/

    struct vvrframework_API Point2D : public Shape
    {
        real x, y;

    protected:
        void drawShape() const override;

    public:
        Point2D() { }

        Point2D(real x, real y, Colour col = Colour())
            : Shape(col)
            , x(x)
            , y(y)
        { }
    };

    struct vvrframework_API LineSeg2D : public Shape
    {
        real x1, y1;
        real x2, y2;

    protected:

        void drawShape() const override;

    public:
        LineSeg2D() { }

        LineSeg2D(real x1, real y1, real x2, real y2, Colour col = Colour())
            : Shape(col)
            , x1(x1)
            , y1(y1)
            , x2(x2)
            , y2(y2)
        { }

        void set(real x1, real y1, real x2, real y2)
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
        Line2D() { }
        Line2D(real x1, real y1, real x2, real y2, Colour col = Colour())
            : LineSeg2D(x1, y1, x2, y2, col)
        { }
    };

    struct vvrframework_API Triangle2D : public Shape
    {
        real x1, y1;
        real x2, y2;
        real x3, y3;

    protected:
        void drawShape() const override;

    public:
        Triangle2D() { filled = false; }
        Triangle2D(real x1, real y1, real x2, real y2, real x3, real y3, Colour col = Colour())
            : Shape(col)
            , x1(x1)
            , y1(y1)
            , x2(x2)
            , y2(y2)
            , x3(x3)
            , y3(y3)
        {
            filled = false;
        }

        real pickdist(int x, int y) const override
        {
            C2DTriangle t(C2DPoint(x1,y1), C2DPoint(x2,y2), C2DPoint(x3,y3));
            return (t.Contains(C2DPoint(x,y))) ? t.GetInCentre().Distance(C2DPoint(x,y)) : -1;
        }

        void set(real x1, real y1, real x2, real y2, real x3, real y3)
        {
            vvr_setmemb(x1);
            vvr_setmemb(y1);
            vvr_setmemb(x2);
            vvr_setmemb(y2);
            vvr_setmemb(x3);
            vvr_setmemb(y3);
        }
    };

    struct vvrframework_API Circle2D : public Shape, public C2DCircle
    {
        vvr_decl_shape(Circle2D, C2DCircle, false)

        Circle2D(real x, real y, real r, Colour col = Colour())
            : Shape(col)
            , C2DCircle(C2DPoint(x,y), r)
        {
            setup();
        }

        void setRange(real from, real to)
        {
            range_from = from;
            range_to = to;
        }

        void setClosedLoop(bool closed)
        {
            closed_loop = closed;
        }

        real pickdist(int x, int y) const override
        {
            real d = GetCentre().Distance(C2DPoint(x,y));
            return d <= GetRadius() ? d : -1.0f;
        }

        real range_from;  // in radians
        real range_to;    // in radians
        bool closed_loop;

    private:
        void drawShape() const override;

        void setup() override
        {
            range_from = 0.0f;
            range_to = math::pi * 2.0f;
            closed_loop = true;
        }
    };

    /*---[Shapes: 3D]-------------------------------------------------------------------*/

    struct vvrframework_API Point3D : public Shape, public math::vec
    {
        vvr_decl_shape(Point3D, math::vec, false)

        Point3D(real x, real y, real z, Colour col = Colour())
            : Shape(col)
            , math::vec{ x, y, z }
        { }

        real pickdist(int x, int y) const override
        {
            real d = this->Distance(vec(x,y,0));
            return (d < PointSize) ? d : -1;
        }

    private:
        void drawShape() const override;
    };

    struct vvrframework_API LineSeg3D : public Shape, public math::LineSegment
    {
        vvr_decl_shape(LineSeg3D, math::LineSegment, false)

        LineSeg3D(real x1, real y1, real z1, real x2, real y2, real z2,
                  Colour col = Colour())
            : Shape(col)
            , LineSegment(math::vec{x1, y1, z1},
                          math::vec{x2, y2, z2})
        { }

        real pickdist(int x, int y) const override
        {
            real d = this->Distance(vec(x, y, 0));
            return d <= Point3D::PointSize ? d : -1.0f;
        }

    private:
        void drawShape() const override;
    };

    struct vvrframework_API Sphere3D : public Shape, public math::Sphere
    {
        vvr_decl_shape(Sphere3D, math::Sphere, false)

        Sphere3D(real x, real y, real z, real r, Colour col = Colour())
            : Shape(col)
            , math::Sphere({x,y,z}, r)
        { }

    private:
        void drawShape() const override;
    };

    struct vvrframework_API Aabb3D : public Shape, public math::AABB
    {
        vvr_decl_shape(Aabb3D, math::AABB, false)

        Aabb3D(real xmin, real ymin, real zmin,
               real xmax, real ymax, real zmax, Colour col = Colour())
            : Shape(col)
            , math::AABB(math::vec{ xmin, ymin, zmin }, math::vec{ xmax, ymax, zmax })
        {
            setup();
        }

        Aabb3D(const std::vector<math::vec> vertices, Colour col = Colour())
            : Shape(col)
            , math::AABB(aabbFromVertices(vertices))
        {
            setup();
        }

        void setTransparency(real a)
        {
            transparency = a;
        }

        real transparency;

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
        Point3D *cornerpts;
        Colour col_edge;
    };

    struct vvrframework_API Triangle3D : public Shape, public math::Triangle
    {
        vvr_decl_shape(Triangle3D, math::Triangle, true)

        Triangle3D(real x1, real y1, real z1,
                   real x2, real y2, real z2,
                   real x3, real y3, real z3, Colour col = Colour())
            : Shape(col, true)
            , math::Triangle(
                math::vec{ x1, y1, z1 },
                math::vec{ x2, y2, z2 },
                math::vec{ x3, y3, z3 })
        {
            setup();
        }

        real pickdist(int x, int y) const override
        {
            vec p(x, y, 0);
            return this->Contains(p)? this->CenterPoint().Distance(p) : -1;
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

    struct vvrframework_API Cylinder3D : public Shape
    {
        Cylinder3D(Colour col=Colour())
            : Shape(col)
        {
            setup();
        }

        Cylinder3D(const vec& org, const vec& dir, real r, real h, Colour col)
            : Shape(col)
            , basecenter(org)
            , normal(dir.Normalized())
            , radius(r)
            , height(h)
        {
            setup();
        }

        math::Circle diskBase() const
        {
            return math::Circle(basecenter, normal, radius);
        }

        math::Circle diskTop() const
        {
            return math::Circle(basecenter + normal*height, normal, radius);
        }

        vec basecenter;
        vec normal;
        real radius;
        real height;
        bool sides;

    private:
        void drawShape() const override;

        void setup() override
        {
            sides = true;
            filled = true;
        }
    };

    /*---[Drawables: Others]------------------------------------------------------------*/

    struct vvrframework_API Ground : public Drawable
    {
        Ground(const real W, const real D, const real B, const real T, Colour colour);
        void draw() const override;

    private:
        std::vector<Triangle3D> m_floor_tris;
        Colour m_col;
    };

    struct vvrframework_API Axes : Drawable
    {
        Axes(real d)
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

    /*---[Canvas]-----------------------------------------------------------------------*/

    class vvrframework_API Canvas : public Drawable
    {
        struct Frame
        {
            Frame(bool show_old = true) : show_old(show_old) { }
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

        Drawable* add(const C2DPoint &p, Colour col = Colour())
        {
            return add(new Point2D(p.x, p.y, col));
        }

        Drawable* add(const C2DPoint &p1, const C2DPoint &p2, Colour col = Colour(), bool inf = false)
        {
            if (inf ){
                return add(new Line2D(p1.x, p1.y, p2.x, p2.y, col));
            }
            else {
                return add(new LineSeg2D(p1.x, p1.y, p2.x, p2.y, col));
            }
        }

        Drawable* add(const C2DLine &line, Colour col = Colour(), bool inf_line = false)
        {
            const C2DPoint &p1 = line.GetPointFrom();
            const C2DPoint &p2 = line.GetPointTo();
            return add(p1, p2, col, inf_line);
        }

        Drawable* add(const C2DCircle &circle, Colour col = Colour(), bool solid = false)
        {
            Shape * s = new Circle2D(circle.GetCentre().x, circle.GetCentre().y, circle.GetRadius(), col);
            s->filled = solid;
            return add(s);
        }

        Drawable* add(const C2DTriangle &tri, Colour col = Colour(), bool solid = false)
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

    /*---[Composite Drawables]----------------------------------------------------------*/

    template <class WholeT, class BlockT, size_t N>
    struct Composite : public Drawable
    {
        static_assert(!std::is_pointer<BlockT>::value, "Don't declare pointers.");
        static_assert(N>1, "N must be 1+");

        std::array<BlockT*, N> blocks;
        WholeT whole;

        template <typename... T>
        Composite(std::array<BlockT*, N> comps, Colour colour)
            : blocks{ comps }
            , whole(assemble(blocks, std::make_index_sequence<N>(), colour))
        {
        }

        Composite(const Composite &other)
            : whole{other.whole}
        {
            for (int i=0; i<N; i++) {
                blocks[i] = new BlockT(*other.blocks[i]);
            }
        }

        void addToCanvas(Canvas &canvas) override
        {
            canvas.add(this);
            for (auto c : blocks) canvas.add(c);
        }

        template<typename Array, std::size_t... I>
        WholeT assemble(const Array& a, std::index_sequence<I...>, Colour col) const
        {
            return WholeT({ *a[I]... }, col);
        }

        template<typename Array, std::size_t... I>
        void update(const Array& a, std::index_sequence<I...>) const
        {
            const_cast<WholeT&>(whole).setGeom({ *a[I]... });
        }

        real pickdist(int x, int y) const override
        {
            return whole.pickdist(x, y);
        }

        void draw() const override
        {
            update(blocks, std::make_index_sequence<N>());
            whole.draw();
        }
    };

    typedef Composite<Triangle3D, Point3D, 3> CompositeTriangle;
    typedef Composite<LineSeg3D, Point3D, 2> CompositeLine;

    /*---[Drawing helpers]--------------------------------------------------------------*/

    vvrframework_API void draw(C2DPointSet &point_set, Colour col = Colour());

    vvrframework_API void draw(C2DLineSet &line_set, Colour col = Colour());

    vvrframework_API void draw(C2DPolygon &polygon, Colour col = Colour(), bool filled = false);

    /*---[MathGeoLib => vvr Converters] (Deprecated. Keep for legacy code)-------------*/

    vvrframework_API Triangle3D math2vvr(const math::Triangle &t, Colour col);

    vvrframework_API LineSeg3D math2vvr(const math::LineSegment &l, Colour col);

    vvrframework_API LineSeg3D math2vvr(const math::Line &l, Colour col);

    vvrframework_API Point3D math2vvr(const math::vec &v, Colour col);

    /*---[Predefined colours]-----------------------------------------------------------*/

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

    /*----------------------------------------------------------------------------------*/
}

#endif
