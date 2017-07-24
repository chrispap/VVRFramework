#include <vvr/drawing.h>
#include <vvr/mesh.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <QtOpenGL>
#include <MathGeoLib.h>

using namespace std;
using namespace vvr;

static void drawSphere(double r, int lats, int longs);
static void drawBox(double x1, double y1, double z1, double x2, double y2, double z2, vvr::Colour col, char alpha);

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

float Shape::LineWidth = 2.2f;
float Shape::PointSize = 7.0f;

void Shape::draw() const 
{
    glPolygonMode(GL_FRONT_AND_BACK, render_solid ? GL_FILL : GL_LINE);
    glColor3ubv(colour.data);
    drawShape();
}

void Point2D::drawShape() const 
{
    glPointSize(PointSize);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex2f(x,y);
    glEnd();
}

void Point3D::drawShape() const 
{
    glPointSize(PointSize);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glVertex3f(x,y,z);
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

void LineSeg3D::drawShape() const 
{
    double dx = x2-x1;
    double dy = y2-y1;

    glLineWidth(LineWidth);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
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

void Triangle3D::drawShape() const
{
    glLineWidth(LineWidth);
    glBegin(GL_TRIANGLES);
    math::vec n = math::Triangle(
        math::vec(x1, y1, z1),
        math::vec(x2, y2, z2),
        math::vec(x3, y3, z3)
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

void Circle2D::drawShape() const 
{
    if (rad_from >= rad_to) {
        std::cerr << "Trying to render circle with [rad_from >= rad_to]" << std::endl;
        return;
    }

    double x_, y_;
    unsigned const numOfSegments = 60;

    glLineWidth(LineWidth);
    glBegin(render_solid? GL_POLYGON : (closed_loop?GL_LINE_LOOP:GL_LINE_STRIP));
    double d_th = (rad_to - rad_from) / numOfSegments;
    for(double theta = rad_from; theta <= rad_to; theta+=d_th) {
        x_ = r * cosf(theta);
        y_ = r * sinf(theta);
        glVertex2f(x + x_, y + y_);
    }
    glEnd();

}

void Sphere3D::drawShape() const 
{
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(rad, rad, rad);
    drawSphere(rad, 12, 15);
    glPopMatrix();
}

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

void Aabb3D::drawShape() const 
{
    drawBox(x1, y1, z1, x2, y2, z2, colour, 0);
    drawBox(x1, y1, z1, x2, y2, z2, colour, 255 - transparency * 255);
}

Obb3D::Obb3D() : num_verts(NumVerticesInTriangulation(1, 1, 1))
{
    colour = vvr::Colour("dd4311");
    col_edge = vvr::Colour();
    verts = new vec[num_verts];
    norms = new vec[num_verts];
    cornerpts = new vvr::Point3D[NumVertices()];
    SetFrom(math::AABB{ { 0, 0, 0 }, { 0, 0, 0 } }, float4x4::identity);
}

Obb3D::~Obb3D()
{
    delete[] verts;
    delete[] norms;
    delete[] cornerpts;
}

void Obb3D::set(const math::AABB& aabb, const float4x4& transform)
{
    SetFrom(aabb, transform);
    Triangulate(1, 1, 1, verts, norms, nullptr, true);
    for (size_t i = 0; i < NumVertices(); ++i) {
        cornerpts[i] = math2vvr(CornerPoint(i), col_edge);
    }
}

void Obb3D::drawShape() const
{
    if (render_solid) {
        for (size_t i = 0; i < num_verts; i += 3) {
            math::Triangle t(verts[i + 0], verts[i + 1], verts[i + 2]);
            math2vvr(t, colour).draw();
        }
    }

    for (size_t i = 0; i < NumEdges(); ++i)
        math2vvr(Edge(i), col_edge).draw();

    auto ptsz_old = vvr::Shape::PointSize;
    vvr::Shape::PointSize = 12;
    for (size_t i = 0; i < NumVertices(); ++i) {
        cornerpts[i].draw();
    }
    vvr::Shape::PointSize = ptsz_old;
}

Ground::Ground(const float W, const float D, const float B, const float T, const vvr::Colour &colour)
    : m_col(colour)
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

void Ground::draw() const
{
    for (int i = 0; i < m_floor_tris.size(); i++)
    {
        vvr::Triangle3D floor_tri = vvr::math2vvr(m_floor_tris.at(i), m_col);
        floor_tri.setRenderSolid(true);
        floor_tri.draw();
    }
}

Canvas::Canvas() : del_on_clear(true)
{
    frames.reserve(16);
    clear();
}

Canvas::~Canvas()
{
    if (del_on_clear) {
        for (int fid = 0; fid < frames.size(); fid++) {
            for (int i = 0; i < frames[fid].drawables.size(); i++) {
                delete frames[fid].drawables[i];
            }
        }
    }
}

Drawable* Canvas::add(Drawable *drawable_ptr)
{
    frames[fid].drawables.push_back(drawable_ptr);
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
        for (unsigned i = 0; i < frames[fi].drawables.size(); i++) {
            if (frames[fi].drawables[i]->isVisible()) {
                frames[fi].drawables[i]->draw();
            }
        }
        fi++;
    }
}

void Canvas::next() 
{
    if (fid<frames.size()-1) fid++;
}

void Canvas::prev() 
{
    if (fid>0) fid--;
}

void Canvas::rew() 
{
    fid = 0;
}

void Canvas::ff() 
{
    fid = frames.size()-1;
}

void Canvas::resize(int i) 
{
    if (i<1 || i > size()-1) return;

    if (del_on_clear) {
        for (int fid = i; fid < frames.size(); fid++) {
            for (int si = 0; si < frames[fid].drawables.size(); si++) {
                delete frames[fid].drawables[si];
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
            for (int si = 0; si < frames[fid].drawables.size(); si++) {
                delete frames[fid].drawables[si];
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
        for (int si = 0; si < frames[fid].drawables.size(); si++) {
            delete frames[fid].drawables[si];
        }
    }

    frames[fid].drawables.clear();
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
                    t.setRenderSolid(true);
                    t.setColour(col);
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

void drawBox(double x1, double y1, double z1, double x2, double y2, double z2, Colour col, char a)
{
    static vec p[8];
    vec *v = p;

    *v++ = vec(x1, y1, z1); //0
    *v++ = vec(x1, y2, z1); //1
    *v++ = vec(x1, y2, z2); //2
    *v++ = vec(x1, y1, z2); //3
    *v++ = vec(x2, y1, z1); //4
    *v++ = vec(x2, y2, z1); //5
    *v++ = vec(x2, y2, z2); //6
    *v++ = vec(x2, y1, z2); //7

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
