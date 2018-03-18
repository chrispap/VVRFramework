#ifndef VVR_DRAWING_H
#define VVR_DRAWING_H

#include "vvrframework_DLL.h"
#include "utils.h"
#include "palette.h"
#include "macros.h"
#include <MathGeoLib.h>
#include <GeoLib.h>
#include <string>
#include <vector>
#include <array>
#include <cstdlib>
#include <utility>

namespace vvr
{
    typedef float real;
    using math::vec;
    struct Canvas;
    struct Drawable;

    /*--------------------------------------------------------------------[Functions]---*/
    math::AABB aabbFromVertices(const std::vector<vec> &vertices);

    VVRFramework_API void draw(C2DPointSet &point_set, Colour col = Colour());

    VVRFramework_API void draw(C2DLineSet &line_set, Colour col = Colour());

    VVRFramework_API void draw(C2DPolygon &polygon, Colour col = Colour(), bool filled = false);

    VVRFramework_API void collect(Canvas&, Drawable*);

    /*--------------------------------------------------------------------[Drawables]---*/
    struct VVRFramework_API Drawable
    {
        virtual ~Drawable() {}
        virtual void draw() const = 0;
        virtual real pickdist(int x, int y) const { return real(-1); }
        virtual real pickdist(const math::Ray&) const { return real(-1); }
        virtual Drawable* clone() { return nullptr; }
        virtual void collect(Canvas &canvas);
        void drawif() const { if (visible) draw(); }
        bool show() { return (visible = true); }
        bool hide() { return (visible = false); }
        bool toggle() { return (visible = !visible); }
        bool visible = true;
    };

    struct VVRFramework_API Shape       : Drawable
    {
        Shape() : filled(false) { }
        Shape(Colour col, bool filled=false) : filled(filled), colour(col) { }
        virtual ~Shape() { }
        virtual void drawShape() const = 0;
        virtual void setup() { }
        void draw() const override;

        static real SetLineWidth(real v) { std::swap(v, LineWidth); return v; }
        static real SetPointSize(real v) { std::swap(v, LineWidth); return v; }

        static real LineWidth;
        static real PointSize;

        bool filled;
        Colour colour;
    };

    struct VVRFramework_API Canvas      : Drawable
    {
    private:
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
        Drawable* add(Drawable *drw);
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

        /* 4 easy add */
        Drawable* add(const C2DPoint &p, Colour col = Colour());
        Drawable* add(const C2DPoint &p1, const C2DPoint &p2, Colour col = Colour(), bool inf = false);
        Drawable* add(const C2DLine &line, Colour col = Colour(), bool inf_line = false);
        Drawable* add(const C2DCircle &circle, Colour col = Colour(), bool solid = false);
        Drawable* add(const C2DTriangle &tri, Colour col = Colour(), bool solid = false);
    };

    /*---[Shapes: 2D]-------------------------------------------------------------------*/
    struct VVRFramework_API Point2D     : Shape
    {
        real x, y;

        Point2D() { }

        Point2D(real x, real y, Colour col = Colour())
            : Shape(col)
            , x(x)
            , y(y)
        { }

    private:
        void drawShape() const override;
    };

    struct VVRFramework_API LineSeg2D   : Shape
    {
        real x1, y1;
        real x2, y2;

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

    private:
        void drawShape() const override;
    };

    struct VVRFramework_API Line2D      : Shape
    {
        real x1, y1;
        real x2, y2;

        Line2D() { }

        Line2D(real x1, real y1, real x2, real y2, Colour col = Colour())
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

    private:
        void drawShape() const override;
    };

    struct VVRFramework_API Triangle2D  : Shape
    {
        real x1, y1;
        real x2, y2;
        real x3, y3;

        Triangle2D()
        {
            filled = false;
        }

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

    private:
        void drawShape() const override;
    };

    struct VVRFramework_API Circle2D    : Shape, C2DCircle
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
    struct VVRFramework_API Point3D     : Shape, math::vec
    {
        vvr_decl_shape(Point3D, vec, false)

        Point3D(real x, real y, real z, Colour col = Colour())
            : Shape(col)
            , vec{ x, y, z }
        { }

        real pickdist(int x, int y) const override
        {
            real d = this->Distance(vec(x,y,0));
            return (d < PointSize) ? d : -1;
        }

    private:
        void drawShape() const override;
    };

    struct VVRFramework_API LineSeg3D   : Shape, math::LineSegment
    {
        vvr_decl_shape(LineSeg3D, math::LineSegment, false)

        LineSeg3D(real x1, real y1, real z1, real x2, real y2, real z2,
                  Colour col = Colour())
            : Shape(col)
            , LineSegment(vec{x1, y1, z1},
                          vec{x2, y2, z2})
        { }

        real pickdist(int x, int y) const override
        {
            real d = this->Distance(vec(x, y, 0));
            return d <= Point3D::PointSize ? d : -1.0f;
        }

    private:
        void drawShape() const override;
    };

    struct VVRFramework_API Sphere3D    : Shape, math::Sphere
    {
        vvr_decl_shape(Sphere3D, math::Sphere, false)

        Sphere3D(real x, real y, real z, real r, Colour col = Colour())
            : Shape(col)
            , math::Sphere({x,y,z}, r)
        { }

    private:
        void drawShape() const override;
    };

    struct VVRFramework_API Aabb3D      : Shape, math::AABB
    {
        vvr_decl_shape(Aabb3D, math::AABB, false)

        Aabb3D(real xmin, real ymin, real zmin,
               real xmax, real ymax, real zmax, Colour col = Colour())
            : Shape(col)
            , math::AABB(vec{ xmin, ymin, zmin }, vec{ xmax, ymax, zmax })
        {
            setup();
        }

        Aabb3D(const std::vector<vec> vertices, Colour col = Colour())
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

    struct VVRFramework_API Obb3D       : Shape, math::OBB
    {
        Obb3D();
        ~Obb3D();
        Obb3D(const Obb3D&) = delete;
        void set(const math::AABB& aabb, const math::float4x4& transform);
        void drawShape() const override;
        Colour col_edge;

    private:
        const size_t num_triverts;
        vec *tv;        // triangle vertices
        vec *tn;        // triangle normals
        Point3D *cp;    // corner points
    };

    struct VVRFramework_API Triangle3D  : Shape, math::Triangle
    {
        vvr_decl_shape(Triangle3D, math::Triangle, true)

        Triangle3D(real x1, real y1, real z1,
                   real x2, real y2, real z2,
                   real x3, real y3, real z3, Colour col = Colour())
            : Shape(col, true)
            , math::Triangle(
                vec{ x1, y1, z1 },
                vec{ x2, y2, z2 },
                vec{ x3, y3, z3 })
        {
            vertex_col[0] = col;
            vertex_col[1] = col;
            vertex_col[2] = col;
            setup();
        }

        real pickdist(int x, int y) const override
        {
            vec p(x, y, 0);
            return this->Contains(p)? this->CenterPoint().Distance(p) : -1;
        }

        real pickdist(const math::Ray &ray) const override
        {
            vec ip;
            bool intr = Intersects(ray, nullptr, &ip);
            return intr ? Distance(ip) : real(-1);
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

    struct VVRFramework_API Cylinder3D  : Shape
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

    /*---[Widgets]----------------------------------------------------------------------*/
    struct VVRFramework_API Ground      : Drawable
    {
        Ground(const real W, const real D, const real B, const real T, Colour colour);
        void draw() const override;

    private:
        std::vector<Triangle3D> m_floor_tris;
        Colour m_col;
    };

    struct VVRFramework_API Axes        : Drawable
    {
        vvr_decl_shared_ptr(Axes)

        Axes(real d=1) { setSize(d); }

        void setSize(real d)
        {
            x = vvr::LineSeg3D(0, 0, 0, d, 0, 0, vvr::red);
            y = vvr::LineSeg3D(0, 0, 0, 0, d, 0, vvr::green);
            z = vvr::LineSeg3D(0, 0, 0, 0, 0, d, vvr::blue);
        }

        virtual void draw() const override
        {
            x.draw();
            y.draw();
            z.draw();
        }

    private:
        LineSeg3D x, y, z;
    };

    /*---[Composite]--------------------------------------------------------------------*/
    template <class WholeT, class BlockT, size_t N>
    struct Composite : public Drawable
    {
        static_assert(!std::is_pointer<BlockT>::value, "Don't declare block type as pointer.");
        static_assert(N>1, "N must be 1+");

        std::array<BlockT*, N> blocks;
        WholeT whole;

        template <typename... T>
        Composite(std::array<BlockT*, N> comps, Colour colour)
            : blocks{ comps }
            , whole{ assemble(std::make_index_sequence<N>(), colour) }
        {
        }

        Composite(const Composite &other) : whole{other.whole}
        {
            for (int i=0; i<N; i++) {
                blocks[i] = new BlockT(*other.blocks[i]);
            }
        }

        template<std::size_t... I>
        WholeT assemble(std::index_sequence<I...>, Colour col) const
        {
            return WholeT({ *blocks[I]... }, col);
        }

        template<std::size_t... I>
        void update(std::index_sequence<I...>) const
        {
            const_cast<WholeT&>(whole).setGeom({ *blocks[I]... });
        }

        void draw() const override
        {
            update(std::make_index_sequence<N>());
            whole.draw();
        }

        real pickdist(int x, int y) const override
        {
            return whole.pickdist(x, y);
        }

        void collect(Canvas &canvas) override
        {
            canvas.add(this);
            for (auto c : blocks) canvas.add(c);
        }
    };

    typedef Composite<Triangle3D, Point3D, 3> CompositeTriangle;

    typedef Composite<LineSeg3D, Point3D, 2> CompositeLine;
    /*----------------------------------------------------------------------------------*/
}

#endif
