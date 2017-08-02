#include <vvr/drawing.h>
#include <vvr/mesh.h>
#include <MathGeoLib.h>
#include <GeoLib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <QtOpenGL>

using vvr::real_t;
using math::vec;

real_t vvr::Shape::LineWidth = 2.2f;
real_t vvr::Shape::PointSize = 7.0f;

const vvr::Colour vvr::red(0xFF, 0x00, 0x00);
const vvr::Colour vvr::blue(0x00, 0x00, 0xFF);
const vvr::Colour vvr::grey(0x66, 0x66, 0x66);
const vvr::Colour vvr::cyan(0x00, 0xFF, 0xFF);
const vvr::Colour vvr::white(0xFF, 0xFF, 0xFF);
const vvr::Colour vvr::green(0x00, 0xFF, 0x00);
const vvr::Colour vvr::black(0x00, 0x00, 0x00);
const vvr::Colour vvr::yellow(0xFF, 0xFF, 0x00);
const vvr::Colour vvr::orange(0xFF, 0x66, 0x00);
const vvr::Colour vvr::magenta(0xFF, 0x00, 0xFF);
const vvr::Colour vvr::darkRed(0x8B, 0x00, 0x00);
const vvr::Colour vvr::darkOrange(0xFF, 0x8C, 0x00);
const vvr::Colour vvr::darkGreen(0x00, 0x64, 0x00);
const vvr::Colour vvr::yellowGreen(0x9A, 0xCD, 0x32);
const vvr::Colour vvr::lilac(0xCD, 0xA9, 0xCD);

static void drawSphere(real_t r, int lats, int longs);
static void drawBox(const vec &p1, const vec &p2, vvr::Colour col, char alpha);

/*--- [Helpers] -----------------------------------------------------------------------*/

math::AABB vvr::aabbFromVertices(const std::vector<vec> &vertices)
{
    vec lo, hi;
    lo.x = lo.y = lo.z = FLT_MAX;
    hi.x = hi.y = hi.z = -FLT_MAX;
    std::vector<vec>::const_iterator vi;
    for (vi = vertices.begin(); vi != vertices.end(); ++vi) {
        if (vi->x > hi.x) hi.x = vi->x;
        else if (vi->x < lo.x) lo.x = vi->x;
        if (vi->y > hi.y) hi.y = vi->y;
        else if (vi->y < lo.y) lo.y = vi->y;
        if (vi->z > hi.z) hi.z = vi->z;
        else if (vi->z < lo.z) lo.z = vi->z;
    }

    return math::AABB(lo, hi);
}

/*--- [Shape] Drawing -----------------------------------------------------------------*/

void vvr::Shape::draw() const 
{
    glPolygonMode(GL_FRONT_AND_BACK, filled ? GL_FILL : GL_LINE);
    glColor4ubv(colour.data);
    drawShape();
}

/*--- [Shape] Drawing 2D --------------------------------------------------------------*/

void vvr::Point2D::drawShape() const 
{
    glPointSize(PointSize);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
}

void vvr::LineSeg2D::drawShape() const
{
    glLineWidth(LineWidth);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void vvr::Line2D::drawShape() const
{
    double dx = x2-x1;
    double dy = y2-y1;

    glLineWidth(LineWidth);
    glBegin(GL_LINES);
    glVertex2f(x1 - 1000*dx, y1 - 1000*dy);
    glVertex2f(x2 + 1000*dx, y2 + 1000*dy);
    glEnd();
}

void vvr::Triangle2D::drawShape() const
{
    glLineWidth(LineWidth);
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void vvr::Circle2D::drawShape() const
{
    assert(range_from < range_to);

    unsigned const numOfSegments = 60;
    const real_t cx = GetCentre().x;
    const real_t cy = GetCentre().y;
    const real_t cr = GetRadius();
    real_t x, y;

    glLineWidth(LineWidth);

    glBegin(filled ? GL_POLYGON : (closed_loop ? GL_LINE_LOOP : GL_LINE_STRIP));
    real_t d_th = (range_to - range_from) / numOfSegments;
    for (real_t theta = range_from; theta <= range_to; theta += d_th) {
        math::SinCos(theta, y, x);
        x *= cr; 
        y *= cr;
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

/*--- [Shape] Drawing 3D --------------------------------------------------------------*/

void vvr::Point3D::drawShape() const
{
    glPointSize(PointSize);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}

void vvr::LineSeg3D::drawShape() const
{
    glLineWidth(LineWidth);
    glBegin(GL_LINES);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glEnd();
}

void vvr::Triangle3D::drawShape() const
{
    glLineWidth(LineWidth);
    glBegin(GL_TRIANGLES);
    vec n = NormalCCW();

    glNormal3fv(n.ptr());

    glColor3ubv(vertex_col[0].data);
    glVertex3fv(a.ptr());

    glColor3ubv(vertex_col[1].data);
    glVertex3fv(b.ptr());

    glColor3ubv(vertex_col[2].data);
    glVertex3fv(c.ptr());

    glEnd();
}

void vvr::Sphere3D::drawShape() const
{
    glPushMatrix();
    glTranslated(pos.x, pos.y, pos.z);
    glScalef(r, r, r);
    drawSphere(r, 12, 15);
    glPopMatrix();
}

void vvr::Aabb3D::drawShape() const
{
    drawBox(minPoint, maxPoint, colour, 0);
    drawBox(minPoint, maxPoint, colour, 255 - transparency * 255);
}

void vvr::Obb3D::drawShape() const
{
    if (filled) 
    {
        for (size_t i = 0; i < num_triverts; i += 3) {
            math::Triangle t(triverts[i + 0], triverts[i + 1], triverts[i + 2]);
            math2vvr(t, colour).draw();
        }
    }

    for (size_t i = 0; i < NumEdges(); ++i) 
        math2vvr(Edge(i), col_edge).draw();
    
    auto ptsz_old = vvr::Shape::PointSize;
    vvr::Shape::PointSize = 12;
    for (size_t i = 0; i < NumVertices(); ++i) 
        cornerpts[i].draw();
    vvr::Shape::PointSize = ptsz_old;
}

vvr::Obb3D::Obb3D() : num_triverts(NumVerticesInTriangulation(1, 1, 1))
{
    colour = vvr::Colour("dd4311");
    col_edge = vvr::Colour();
    triverts = new vec[num_triverts];
    trinorms = new vec[num_triverts];
    cornerpts = new vvr::Point3D[NumVertices()];
    SetFrom(math::AABB{ vec{ 0, 0, 0 }, vec{ 0, 0, 0 } }, math::float4x4::identity);
}

vvr::Obb3D::~Obb3D()
{
    delete[] triverts;
    delete[] trinorms;
    delete[] cornerpts;
}

void vvr::Obb3D::set(const math::AABB& aabb, const math::float4x4& transform)
{
    SetFrom(aabb, transform);
    Triangulate(1, 1, 1, triverts, trinorms, nullptr, true);
    for (size_t i = 0; i < NumVertices(); ++i) 
        cornerpts[i].set(CornerPoint(i));
}

void vvr::Ground::draw() const
{
    for (auto &t : m_floor_tris) t.draw();
}

vvr::Ground::Ground(const real_t W, const real_t D, const real_t B, const real_t T, const vvr::Colour &col)
    : m_col(col)
{
    const vec vA(-W / 2, B, -D / 2);
    const vec vB(+W / 2, B, -D / 2);
    const vec vC(+W / 2, B, +D / 2);
    const vec vD(-W / 2, B, +D / 2);
    const vec vE(-W / 2, T, -D / 2);
    const vec vF(+W / 2, T, -D / 2);

    m_floor_tris.push_back(vvr::Triangle3D(math::Triangle(vB, vA, vD), col));
    m_floor_tris.push_back(vvr::Triangle3D(math::Triangle(vB, vD, vC), col));
    m_floor_tris.push_back(vvr::Triangle3D(math::Triangle(vF, vE, vA), col));
    m_floor_tris.push_back(vvr::Triangle3D(math::Triangle(vF, vA, vB), col));
}

/*--- [Canvas] ------------------------------------------------------------------------*/

vvr::Canvas::Canvas() : fid(0) , del_on_clear(true)
{
    frames.reserve(16);
    frames.push_back(Frame(false));
}

vvr::Canvas::~Canvas()
{
    if (del_on_clear) {
        for (int fid = 0; fid < frames.size(); fid++) {
            for (int i = 0; i < frames[fid].drvec.size(); i++) {
                delete frames[fid].drvec[i];
            }
        }
    }
}

vvr::Drawable* vvr::Canvas::add(vvr::Drawable *drawable_ptr)
{
    frames[fid].drvec.push_back(drawable_ptr);
    return drawable_ptr;
}

void vvr::Canvas::newFrame(bool show_old_frames)
{
    frames.push_back(Frame(show_old_frames));
    ff();
}

void vvr::Canvas::draw() const
{
    int fi = (int) fid;
    while (frames[fi].show_old && --fi >= 0);
    while(fi <= fid) {
        for (size_t i = 0; i < frames[fi].drvec.size(); i++) {
            frames[fi].drvec[i]->drawif();
        }
        fi++;
    }
}

void vvr::Canvas::resize(int i)
{
    if (i<1 || i > size()-1) 
        return;

    if (del_on_clear) {
        for (int fid = i; fid < frames.size(); fid++) {
            for (int si = 0; si < frames[fid].drvec.size(); si++) {
                delete frames[fid].drvec[si];
            }
        }
    }

    frames.resize(i);
    fid=i-1;
}

void vvr::Canvas::clear()
{
    if (del_on_clear) {
        for (int fid = 0; fid < frames.size(); fid++) {
            for (int si = 0; si < frames[fid].drvec.size(); si++) {
                delete frames[fid].drvec[si];
            }
        }
    }

    frames.clear();
    frames.push_back(Frame(false));
    fid=0;
}

void vvr::Canvas::clearFrame()
{
    if (del_on_clear) {
        for (int si = 0; si < frames[fid].drvec.size(); si++) {
            delete frames[fid].drvec[si];
        }
    }

    frames[fid].drvec.clear();
}

/*--- [Drawing helpers] ---------------------------------------------------------------*/

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
    bool err = false;

    if (filled)
    {
        if (!polygon.IsConvex()) {
            err = !polygon.CreateConvexSubAreas();
        }
        if (!err) 
        {
            C2DPolygonSet polyset;
            polygon.GetConvexSubAreas(polyset);
            for (int i = 0; i < polyset.size(); i++) {
                C2DPolygon &convpoly = *polyset.GetAt(i);
                C2DPoint convpoly_centroid = convpoly.GetCentroid();
                for (int j = 0; j < convpoly.GetLines().size(); j++) {
                    Triangle2D t(
                        convpoly.GetLines().GetAt(j)->GetPointFrom().x,
                        convpoly.GetLines().GetAt(j)->GetPointFrom().y,
                        convpoly.GetLines().GetAt(j)->GetPointTo().x,
                        convpoly.GetLines().GetAt(j)->GetPointTo().y,
                        convpoly_centroid.x, convpoly_centroid.y);
                    t.filled = true;
                    t.colour = col;
                    t.draw();
                }
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

    if (err) {
        std::cerr << "Polygon Invalid. Cannot render." << std::endl;
    }
}

/*--- [MathGeoLib => vvr Converters] --------------------------------------------------*/

vvr::Triangle3D vvr::math2vvr(const math::Triangle &t, const vvr::Colour &col)
{
    return vvr::Triangle3D(t, col);
}

vvr::LineSeg3D vvr::math2vvr(const math::LineSegment &l, const vvr::Colour &col)
{
    return vvr::LineSeg3D(l, col);
}

vvr::LineSeg3D vvr::math2vvr(const math::Line &l, const vvr::Colour &col)
{
    return vvr::LineSeg3D(l.ToLineSegment(1000), col);
}

vvr::Point3D vvr::math2vvr(const vec &v, const vvr::Colour &col)
{
    return vvr::Point3D(v, col);
}

/*--- [Static drawing functions] ------------------------------------------------------*/

void drawSphere(real_t r, int lats, int longs)
{
    int i, j;
    for (i = 0; i <= lats; i++) {
        real_t lat0 = M_PI * (-0.5 + (real_t)(i - 1) / lats);
        real_t z0 = sin(lat0);
        real_t zr0 = cos(lat0);

        real_t lat1 = M_PI * (-0.5 + (real_t)i / lats);
        real_t z1 = sin(lat1);
        real_t zr1 = cos(lat1);
        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= longs; j++) {
            real_t lng = 2 * M_PI * (real_t)(j - 1) / longs;
            real_t x = cos(lng);
            real_t y = sin(lng);
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(x * zr0, y * zr0, z0);
            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(x * zr1, y * zr1, z1);
        }
        glEnd();
    }
}

void drawBox(const vec &p1, const vec &p2, vvr::Colour col, char a)
{
    static vec p[8];
    vec *v = p;

    *v++ = vec(p1.x, p1.y, p1.z); //0
    *v++ = vec(p1.x, p2.y, p1.z); //1
    *v++ = vec(p1.x, p2.y, p2.z); //2
    *v++ = vec(p1.x, p1.y, p2.z); //3
    *v++ = vec(p2.x, p1.y, p1.z); //4
    *v++ = vec(p2.x, p2.y, p1.z); //5
    *v++ = vec(p2.x, p2.y, p2.z); //6
    *v++ = vec(p2.x, p1.y, p2.z); //7

    glPolygonMode(GL_FRONT_AND_BACK, a ? GL_FILL : GL_LINE);
    glBegin(GL_QUADS);

    if (a) {
        glColor4ub(col.r, col.g, col.b, a);
    }
    else {
        glColor3ubv(col.data);
    }

    glVertex3fv(p[0].ptr());
    glVertex3fv(p[1].ptr());
    glVertex3fv(p[2].ptr());
    glVertex3fv(p[3].ptr());

    glVertex3fv(p[1].ptr());
    glVertex3fv(p[2].ptr());
    glVertex3fv(p[6].ptr());
    glVertex3fv(p[5].ptr());

    glVertex3fv(p[4].ptr());
    glVertex3fv(p[5].ptr());
    glVertex3fv(p[6].ptr());
    glVertex3fv(p[7].ptr());

    glVertex3fv(p[0].ptr());
    glVertex3fv(p[4].ptr());
    glVertex3fv(p[7].ptr());
    glVertex3fv(p[3].ptr());

    glVertex3fv(p[2].ptr());
    glVertex3fv(p[3].ptr());
    glVertex3fv(p[7].ptr());
    glVertex3fv(p[6].ptr());

    glVertex3fv(p[0].ptr());
    glVertex3fv(p[1].ptr());
    glVertex3fv(p[5].ptr());
    glVertex3fv(p[4].ptr());

    glEnd();
}

/*-------------------------------------------------------------------------------------*/
