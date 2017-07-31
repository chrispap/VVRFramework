#include <vvr/drawing.h>
#include <vvr/mesh.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <QtOpenGL>
#include <MathGeoLib.h>

using namespace std;
using namespace vvr;

const Colour vvr::red(0xFF, 0x00, 0x00);
const Colour vvr::blue(0x00, 0x00, 0xFF);
const Colour vvr::grey(0x66, 0x66, 0x66);
const Colour vvr::cyan(0x00, 0xFF, 0xFF);
const Colour vvr::white(0xFF, 0xFF, 0xFF);
const Colour vvr::green(0x00, 0xFF, 0x00);
const Colour vvr::black(0x00, 0x00, 0x00);
const Colour vvr::yellow(0xFF, 0xFF, 0x00);
const Colour vvr::orange(0xFF, 0x66, 0x00);
const Colour vvr::magenta(0xFF, 0x00, 0xFF);
const Colour vvr::darkRed(0x8B, 0x00, 0x00);
const Colour vvr::darkOrange(0xFF, 0x8C, 0x00);
const Colour vvr::darkGreen(0x00, 0x64, 0x00);
const Colour vvr::yellowGreen(0x9A, 0xCD, 0x32);
const Colour vvr::lilac(0xCD, 0xA9, 0xCD);

float Shape::LineWidth = 2.2f;
float Shape::PointSize = 7.0f;

static void drawSphere(real_t r, int lats, int longs);
static void drawBox(const vec &p1, const vec &p2, vvr::Colour col, char alpha);

void Shape::draw() const 
{
    glPolygonMode(GL_FRONT_AND_BACK, filled ? GL_FILL : GL_LINE);
    glColor4ubv(colour.data);
    drawShape();
}

/*--- [Shape] 2D Drawing ---------------------------------------------------------------*/

void Point2D::drawShape() const 
{
    glPointSize(PointSize);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
}

void LineSeg2D::drawShape() const
{
    glLineWidth(LineWidth);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void Line2D::drawShape() const 
{
    double dx = x2-x1;
    double dy = y2-y1;

    glLineWidth(LineWidth);
    glBegin(GL_LINES);
    glVertex2f(x1 - 1000*dx, y1 - 1000*dy);
    glVertex2f(x2 + 1000*dx, y2 + 1000*dy);
    glEnd();
}

void Triangle2D::drawShape() const
{
    glLineWidth(LineWidth);
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void Circle2D::drawShape() const
{
    if (rad_from >= rad_to) {
        std::cerr << "Trying to render circle with [rad_from >= rad_to]" << std::endl;
        return;
    }

    double x_, y_;
    unsigned const numOfSegments = 60;

    glLineWidth(LineWidth);
    glBegin(filled ? GL_POLYGON : (closed_loop ? GL_LINE_LOOP : GL_LINE_STRIP));
    double d_th = (rad_to - rad_from) / numOfSegments;
    for (double theta = rad_from; theta <= rad_to; theta += d_th) {
        x_ = r * cosf(theta);
        y_ = r * sinf(theta);
        glVertex2f(x + x_, y + y_);
    }
    glEnd();

}

/*--- [Shape] 3D Drawing ---------------------------------------------------------------*/

void Point3D::drawShape() const
{
    glPointSize(PointSize);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex3f(x, y, z);
    glEnd();
}

void LineSeg3D::drawShape() const 
{
    glLineWidth(LineWidth);
    glBegin(GL_LINES);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);
    glEnd();
}

void Triangle3D::drawShape() const
{
    glLineWidth(LineWidth);
    glBegin(GL_TRIANGLES);
    math::vec n = math::Triangle(a, b, c).NormalCW();

    glNormal3fv(n.ptr());

    glColor3ubv(vertex_col[0].data);
    glVertex3fv(a.ptr());

    glColor3ubv(vertex_col[1].data);
    glVertex3fv(b.ptr());

    glColor3ubv(vertex_col[2].data);
    glVertex3fv(c.ptr());

    glEnd();
}

void Sphere3D::drawShape() const 
{
    glPushMatrix();
    glTranslated(pos.x, pos.y, pos.z);
    glScalef(r, r, r);
    drawSphere(r, 12, 15);
    glPopMatrix();
}

void Aabb3D::drawShape() const
{
    drawBox(vec{ x1, y1, z1 }, vec{ x2, y2, z2 }, colour, 0);
    drawBox(vec{ x1, y1, z1 }, vec{ x2, y2, z2 }, colour, 255 - transparency * 255);
}

void Obb3D::drawShape() const
{
    if (filled) 
    {
        for (size_t i = 0; i < num_verts; i += 3) {
            math::Triangle t(verts[i + 0], verts[i + 1], verts[i + 2]);
            math2vvr(t, colour).draw();
        }
    }

    for (size_t i = 0; i < NumEdges(); ++i) {
        math2vvr(Edge(i), col_edge).draw();
    }

    auto ptsz_old = vvr::Shape::PointSize;
    vvr::Shape::PointSize = 12;
    
    for (size_t i = 0; i < NumVertices(); ++i) {
        cornerpts[i]->draw();
    }

    vvr::Shape::PointSize = ptsz_old;
}

void Ground::draw() const
{
    for (int i = 0; i < m_floor_tris.size(); i++)
    {
        vvr::Triangle3D floor_tri = vvr::math2vvr(m_floor_tris.at(i), m_col);
        floor_tri.filled = true;
        floor_tri.draw();
    }
}

/*--- [Ctors/Dtors] -------------------------------------------------------------------*/

Aabb3D::Aabb3D(const std::vector<vec> vertices, const Colour &col)
    : Shape(col)
    , transparency(0)
{
    math::AABB aabb = aabbFromVertices(vertices);

    x1 = aabb.minPoint.x;
    y1 = aabb.minPoint.y;
    z1 = aabb.minPoint.z;

    x2 = aabb.maxPoint.x;
    y2 = aabb.maxPoint.y;
    z2 = aabb.maxPoint.z;
}

Obb3D::Obb3D() : num_verts(NumVerticesInTriangulation(1, 1, 1))
{
    colour = vvr::Colour("dd4311");
    col_edge = vvr::Colour();
    verts = new vec[num_verts];
    norms = new vec[num_verts];
    cornerpts.resize(NumVertices());
    for (size_t i = 0; i < NumVertices(); ++i) {
        cornerpts[i] = new Point3D(0, 0, 0, col_edge);
    }
    SetFrom(math::AABB{ { 0, 0, 0 }, { 0, 0, 0 } }, float4x4::identity);
}

Obb3D::~Obb3D()
{
    delete[] verts;
    delete[] norms;
    for (size_t i = 0; i < NumVertices(); ++i) {
        delete cornerpts[i];
    }
}

void Obb3D::set(const math::AABB& aabb, const float4x4& transform)
{
    SetFrom(aabb, transform);
    Triangulate(1, 1, 1, verts, norms, nullptr, true);
    for (size_t i = 0; i < NumVertices(); ++i) {
        *static_cast<vec*>(cornerpts[i]) = CornerPoint(i);
    }
}

Ground::Ground(const float W, const float D, const float B, const float T, const vvr::Colour &col)
    : m_col(col)
{
    const vec vA(-W / 2, B, -D / 2);
    const vec vB(+W / 2, B, -D / 2);
    const vec vC(+W / 2, B, +D / 2);
    const vec vD(-W / 2, B, +D / 2);
    const vec vE(-W / 2, T, -D / 2);
    const vec vF(+W / 2, T, -D / 2);

    m_floor_tris.push_back(math::Triangle(vB, vA, vD));
    m_floor_tris.push_back(math::Triangle(vB, vD, vC));
    m_floor_tris.push_back(math::Triangle(vF, vE, vA));
    m_floor_tris.push_back(math::Triangle(vF, vA, vB));
}

/*--- [Canvas] ------------------------------------------------------------------------*/

Canvas::Canvas() : fid(0) , del_on_clear(true)
{
    frames.reserve(16);
    frames.push_back(Frame(false));
}

Canvas::~Canvas()
{
    if (del_on_clear) {
        for (int fid = 0; fid < frames.size(); fid++) {
            for (int i = 0; i < frames[fid].drvec.size(); i++) {
                delete frames[fid].drvec[i];
            }
        }
    }
}

Drawable* Canvas::add(Drawable *drawable_ptr)
{
    frames[fid].drvec.push_back(drawable_ptr);
    return drawable_ptr;
}

void Canvas::newFrame(bool show_old_frames) 
{
    frames.push_back(Frame(show_old_frames));
    ff();
}

void Canvas::draw() const
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

void Canvas::resize(int i) 
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

void Canvas::clear()
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

void Canvas::clearFrame()
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

/*--- [vvr Converters] ----------------------------------------------------------------*/

vvr::Triangle3D vvr::math2vvr(const math::Triangle &t, const vvr::Colour &col)
{
    return vvr::Triangle3D(
        t.a.x, t.a.y, t.a.z,
        t.c.x, t.c.y, t.c.z,
        t.b.x, t.b.y, t.b.z,
        col);
}

vvr::LineSeg3D vvr::math2vvr(const math::LineSegment &l, const vvr::Colour &col)
{
    return vvr::LineSeg3D(
        l.a.x, l.a.y, l.a.z,
        l.b.x, l.b.y, l.b.z,
        col);
}

vvr::LineSeg3D vvr::math2vvr(const math::Line &l, const vvr::Colour &col)
{
    const auto &lseg(l.ToLineSegment(1000));
    return math2vvr(lseg, col);
}

vvr::Point3D vvr::math2vvr(const math::vec &v, const vvr::Colour &col)
{
    return vvr::Point3D(v.x, v.y, v.z, col);
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

void drawBox(const vec &p1, const vec &p2, Colour col, char a)
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
