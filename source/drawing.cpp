#include <vvr/drawing.h>
#include <vvr/mesh.h>
#include <MathGeoLib.h>
#include <GeoLib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <gl.h>

using vvr::real;
using math::vec;

real vvr::Shape::LineWidth = 2.2f;
real vvr::Shape::PointSize = 7.0f;

/*---[Helpers]---*/

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

void drawSphere(real r, int lats, int longs)
{
    int i, j;
    for (i = 0; i <= lats; i++) {
        real lat0 = M_PI * (-0.5 + (real)(i - 1) / lats);
        real z0 = sin(lat0);
        real zr0 = cos(lat0);

        real lat1 = M_PI * (-0.5 + (real)i / lats);
        real z1 = sin(lat1);
        real zr1 = cos(lat1);
        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= longs; j++) {
            real lng = 2 * M_PI * (real)(j - 1) / longs;
            real x = cos(lng);
            real y = sin(lng);
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

void vvr::draw(C2DPointSet &point_set, Colour col)
{
    /* Draw point cloud */
    for (size_t i = 0; i < point_set.size(); i++) {
        Point2D(
            point_set.GetAt(i)->x,
            point_set.GetAt(i)->y,
            col).draw();
    }
}

void vvr::draw(C2DLineSet  &line_set, Colour col)
{
    for (size_t i = 0; i < line_set.size(); i++) {
        LineSeg2D(
            line_set.GetAt(i)->GetPointFrom().x,
            line_set.GetAt(i)->GetPointFrom().y,
            line_set.GetAt(i)->GetPointTo().x,
            line_set.GetAt(i)->GetPointTo().y,
            col).draw();
    }
}

void vvr::draw(C2DPolygon  &polygon, Colour col, bool filled)
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
            for (size_t i = 0; i < polyset.size(); i++) {
                C2DPolygon &convpoly = *polyset.GetAt(i);
                C2DPoint convpoly_centroid = convpoly.GetCentroid();
                for (size_t j = 0; j < convpoly.GetLines().size(); j++) {
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
        for (size_t i = 0; i < polygon.GetLines().size(); i++) {
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

void vvr::Drawable::addToCanvas(Canvas &canvas)
{
    canvas.add(this);
}

/*---[Shape: Drawing]---*/

void vvr::Shape::draw() const
{
    glPolygonMode(GL_FRONT_AND_BACK, filled ? GL_FILL : GL_LINE);
    glColor4ubv(colour.data);
    drawShape();
}

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
    glVertex2f(x1 - 999999 * dx, y1 - 999999 * dy);
    glVertex2f(x2 + 999999 * dx, y2 + 999999 * dy);
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
    const real cx = GetCentre().x;
    const real cy = GetCentre().y;
    const real cr = GetRadius();
    real x, y;

    glLineWidth(LineWidth);

    glBegin(filled ? GL_POLYGON : (closed_loop ? GL_LINE_LOOP : GL_LINE_STRIP));
    real d_th = (range_to - range_from) / numOfSegments;
    for (real theta = range_from; theta <= range_to; theta += d_th) {
        math::SinCos(theta, y, x);
        x *= cr;
        y *= cr;
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

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

void vvr::Cylinder3D::drawShape() const
{
    auto m = math::float4x4::RotateFromTo({ 0,0,1 }, normal);
    m.SetTranslatePart(basecenter);
    m.Transpose();

    glPushMatrix();
    glMultMatrixf(m.ptr());

    /* Base */
    glBegin(GL_POLYGON);
    vec normalinv(-normal);
    glNormal3fv(normalinv.ptr());
    for (int i = 0; i <= 24; ++i) {
        real x, y;
        math::SinCos(math::pi * 2 / 24 * i, y, x);
        glVertex3f(x*radius, y*radius, 0);
    }
    glEnd();

    /* Sides */
    if (sides)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int i = 0; i <= 24; ++i) {
            real x, y;
            math::SinCos(math::pi * 2 / 24 * i, y, x);
            glNormal3f(x, y, 0);
            glVertex3f(x*radius, y*radius, 0);
            glVertex3f(x*radius, y*radius, -height);
            math::SinCos(math::pi * 2 / 24 * (i + 1), y, x);
            glNormal3f(x, y, 0);
            glVertex3f(x*radius, y*radius, 0);
            glVertex3f(x*radius, y*radius, -height);
        }
        glEnd();
    }

    /* Top */
    glNormal3fv(normal.ptr());
    glBegin(GL_POLYGON);
    for (int i = 0; i <= 24; ++i) {
        real x, y;
        math::SinCos(math::pi * 2 / 24 * i, y, x);
        glVertex3f(x*radius, y*radius, -height);
    }
    glEnd();

    glPopMatrix();
}

void vvr::Sphere3D::drawShape() const
{
    glPushMatrix();
    glTranslatef(pos.x, pos.y, pos.z);
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
            vvr::Triangle3D t(math::Triangle(tv[i + 0], 
                tv[i + 1], tv[i + 2]), colour);
            t.draw();
        }
    }

    for (int i = 0; i < NumEdges(); ++i) {
        LineSeg3D(Edge(i), col_edge).draw();
    }

    const auto ptsz_old = vvr::Shape::PointSize;
    vvr::Shape::PointSize = 12;
    for (int i = 0; i < NumVertices(); ++i) {
        cp[i].draw();
    }
    vvr::Shape::PointSize = ptsz_old;
}

vvr::Obb3D::Obb3D() : num_triverts(NumVerticesInTriangulation(1, 1, 1))
{
    colour = vvr::Colour("dd4311");
    col_edge = vvr::Colour();
    tv = new vec[num_triverts];
    tn = new vec[num_triverts];
    cp = new vvr::Point3D[NumVertices()];
    SetFrom(math::AABB{ vec{ 0, 0, 0 }, vec{ 0, 0, 0 } }, math::float4x4::identity);
}

vvr::Obb3D::~Obb3D()
{
    delete[] tv;
    delete[] tn;
    delete[] cp;
}

void vvr::Obb3D::set(const math::AABB& aabb, const math::float4x4& transform)
{
    SetFrom(aabb, transform);
    Triangulate(1, 1, 1, tv, tn, nullptr, true);
    for (size_t i = 0; i < NumVertices(); ++i) {
        cp[i].setGeom(CornerPoint(i));
    }
}

void vvr::Ground::draw() const
{
    for (auto &t : m_floor_tris) t.draw();
}

vvr::Ground::Ground(const real W, const real D, const real B, const real T, Colour col)
{
    m_col = col;

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

/*---[Canvas]---*/

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

vvr::Drawable* vvr::Canvas::add(const C2DPoint &p, Colour col)
{
    return add(new Point2D(p.x, p.y, col));
}

vvr::Drawable* vvr::Canvas::add(const C2DPoint &p1, const C2DPoint &p2, Colour col, bool inf)
{
    if (inf ){
        return add(new Line2D(p1.x, p1.y, p2.x, p2.y, col));
    }
    else {
        return add(new LineSeg2D(p1.x, p1.y, p2.x, p2.y, col));
    }
}

vvr::Drawable* vvr::Canvas::add(const C2DLine &line, Colour col, bool inf_line)
{
    const C2DPoint &p1 = line.GetPointFrom();
    const C2DPoint &p2 = line.GetPointTo();
    return add(p1, p2, col, inf_line);
}

vvr::Drawable* vvr::Canvas::add(const C2DCircle &circle, Colour col, bool solid)
{
    auto drw = new Circle2D(circle.GetCentre().x, circle.GetCentre().y, circle.GetRadius(), col);
    drw->filled = solid;
    return add(drw);
}

vvr::Drawable* vvr::Canvas::add(const C2DTriangle &tri, Colour col, bool solid)
{
    auto drw = new Triangle2D(
        tri.GetPoint1().x,
        tri.GetPoint1().y,
        tri.GetPoint2().x,
        tri.GetPoint2().y,
        tri.GetPoint3().x,
        tri.GetPoint3().y,
        col);
    drw->filled = solid;
    return add(drw);
}

/*--------------------------------------------------------------------------------------*/
