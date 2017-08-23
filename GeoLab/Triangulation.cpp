#ifndef SCENE_TRIANGULATION_H
#define SCENE_TRIANGULATION_H

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/utils.h>
#include <GeoLib.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <ctime>

struct Tri;

class TriangulationScene : public vvr::Scene
{
public:
    TriangulationScene();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected:
    void draw() override;
    void reset() override;
    void resize() override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;

private:
    void processPoint(C2DPoint* const p);

private:
    vvr::Canvas m_canvas;
    C2DPointSet m_pts;
    std::vector<Tri> m_tris;
    int m_style_flag;
    float m_lw_canvas;
    float m_lw_tris;
    float m_sz_pt;
};

/**
* Struct representing a triangle with pointers to its 3 vertices
*/
struct Tri
{
    C2DPoint *v1;
    C2DPoint *v2;
    C2DPoint *v3;
    float area;

    Tri(C2DPoint *v1, C2DPoint *v2, C2DPoint *v3) : v1(v1), v2(v2), v3(v3) {
        area = to_C2D().GetArea();
    }

    C2DTriangle to_C2D() const { return C2DTriangle(*v1, *v2, *v3); }

    vvr::Triangle2D to_vvr(vvr::Colour col = vvr::black, bool filled = false) const {
        vvr::Triangle2D t(v1->x, v1->y, v2->x, v2->y, v3->x, v3->y, col);
        t.filled = filled;
        return t;
    }

    bool operator < (const Tri& other) const {
        if (area != other.area) return (area < other.area);
        else if (v1 != other.v1) return v1 < other.v1;
        else if (v2 != other.v2) return v2 < other.v2;
        else if (v3 != other.v3) return v3 < other.v3;
        else return false;
    }
};

C2DCircle GetCircumCircle(const C2DTriangle &t);

bool IsDelaunay(const C2DTriangle &t, const C2DPointSet &pset);

bool FindAdjacentTriangle(std::vector<Tri> &tris, C2DPoint *p1, C2DPoint *p2, unsigned *tri_adj_index, C2DPoint **opposite_vertex);

void FindViolations(std::vector<Tri> &tris, const C2DPointSet &ptset, std::vector<unsigned> &violations);

void ShowViolations(std::vector<Tri> &tris, const std::vector<unsigned> &violations, vvr::Canvas &canvas, const vvr::Colour &col);

#endif // SCENE_TRIANGULATION_H
#include <vvr/utils.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <ctime>

#define FLAG_SHOW_CANVAS 1

using namespace std;
using namespace vvr;

TriangulationScene::TriangulationScene()
{
    m_bg_col = Colour(0x44, 0x44, 0x44);
    m_create_menus = false;
    m_lw_canvas = 2;
    m_lw_tris = 3;
    m_sz_pt = 10;
}

void TriangulationScene::reset()
{
    Scene::reset();
    m_style_flag = 0;
    m_style_flag |= FLAG_SHOW_CANVAS;
    m_canvas.clear();
    m_tris.clear();
    m_pts.DeleteAll();

    const int W = getViewportWidth() * 0.9;
    const int H = getViewportHeight()  * 0.9;

    C2DPoint* A = new C2DPoint(-W / 2, -H / 2);
    C2DPoint* B = new C2DPoint(-W / 2, +H / 2);
    C2DPoint* C = new C2DPoint(+W / 2, +H / 2);
    C2DPoint* D = new C2DPoint(+W / 2, -H / 2);

    m_pts.Add(A);
    m_pts.Add(B);
    m_pts.Add(C);
    m_pts.Add(D);
    m_tris.push_back(Tri(A, B, C));
    m_tris.push_back(Tri(A, C, D));
}

void TriangulationScene::resize()
{
    static bool first_pass = true;
    if (first_pass) reset();
    first_pass = false;
}

void TriangulationScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    processPoint(new C2DPoint(x, y));
}

void TriangulationScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    if (m_pts.GetLast()->Distance(C2DPoint(x, y)) > 80)
        processPoint(new C2DPoint(x, y));
}

void TriangulationScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
    case 'c': m_style_flag ^= FLAG_SHOW_CANVAS; break;
    }
}

void TriangulationScene::mouseWheel(int dir, int modif)
{
    if (shiftDown(modif))
    {
        m_lw_canvas *= pow(1.2f, dir);
        m_lw_tris *= pow(1.2f, dir);
        m_sz_pt *= pow(1.2f, dir);
    }
    else
    {
        Scene::mouseWheel(dir, modif);
    }
}

void TriangulationScene::draw()
{
    enterPixelMode();

    //! Draw violations and anything else added to canvas
    if (m_style_flag & FLAG_SHOW_CANVAS) {
        Shape::LineWidth = m_lw_canvas;
        m_canvas.draw();
    }

    //! Draw triangles
    Shape::LineWidth = m_lw_tris;

    vector<Tri>::const_iterator tri_iter;
    for (tri_iter = m_tris.begin(); tri_iter != m_tris.end(); ++tri_iter) {
        tri_iter->to_vvr(vvr::black).draw();
    }

    //! Draw points
    Shape::PointSize = m_sz_pt;
    vvr::draw(m_pts, vvr::red);

    exitPixelMode();
}

int main(int argc, char* argv[])
{
    try
    {
        return vvr::mainLoop(argc, argv, new TriangulationScene);
    }
    catch (std::string exc)
    {
        cerr << exc << endl;
        return 1;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
//! Task Methods / Functions
/////////////////////////////////////////////////////////////////////////////////////////

void TriangulationScene::processPoint(C2DPoint* const p)
{
    //! [Check 1]
    //! Check whether a point already exists in the same coords.

    for (size_t i = 0; i < m_pts.size(); i++)
    {
        if (m_pts.GetAt(i)->x == p->x &&
            m_pts.GetAt(i)->y == p->y)
        {
            delete p;
            return;
        }
    }

    //! Find enclosing triangle.
    unsigned i_enclosing;
    unsigned count_enclosing = 0;
    for (int i = 0; i < m_tris.size(); i++) {
        if (m_tris[i].to_C2D().Contains(*p)) {
            count_enclosing++;
            i_enclosing = i;
        }
    }

    //! [Check 2]
    //! If no enclosing triangle was found.
    //! Or if more than one were found.

    if (count_enclosing != 1) {
        delete p;
        return;
    }

    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!  - Create the 3 subdivision triangles.
    //!  - Chech the 3 new triangles for Delaunay violations.
    //!  - Vreite to geitoniko (adjacent) trigwno tou tri_check.
    //!  - Kante to flip.
    //!
    //! HINTS:
    //!  - Remove the enclosing triangle
    //!
    //!  - Store the 3 new triangles which will replace old one.
    //!    Store them *AFTER* the flipping step, in order to avoid finding one of these
    //!    as adjacent triangle.
    //!
    //!  - To delete a triangle from `m_tris` :
    //!      m_tris.erase(m_tris.begin() + index_trigwnou_pros_diagrafh);
    //!
    //!  - To add a filled `Tri` object to the canvas:
    //!      m_canvas.add(new Triangle2D(tri.to_vvr(color, true)));
    //!
    //!  - Kathe trigwnno exei eite 3 geitonika, eite 2 an prokeitai gia trigwno
    //!    pou vrisketai sta oria (=> anikei merikws sto Convex Hull)
    //!
    //!  - bool adj_exists = FindAdjacentTriangle(m_tris, edge_start, edge_end, &tri_adjacent, &v_opposite);
    //!
    //! VARIABLES:
    //!
    //!  - C2DPoint* p;             // New point
    //!  - Tri &tri_enclosing       // Enclosing triangle of the new point
    //!  - Tri tris_new[3];         // The 3 triangles that will replace the enclosing.
    //!  - Tri *tri_adjacent        // The adjacent triangle
    //!  - C2DPoint *v_opposite     // The opposite vertex of the adjacent triangle
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    vector<Tri> tris_new;
    Tri &tri_enclosing = m_tris[i_enclosing];
    tris_new.push_back(Tri(p, tri_enclosing.v1, tri_enclosing.v2));
    tris_new.push_back(Tri(p, tri_enclosing.v2, tri_enclosing.v3));
    tris_new.push_back(Tri(p, tri_enclosing.v3, tri_enclosing.v1));

    //! [Check 3]
    //! Check if any of the 3 triangles are colinear. (Use GeoLib's function)

    if (tris_new[0].to_C2D().Collinear() ||
        tris_new[1].to_C2D().Collinear() ||
        tris_new[2].to_C2D().Collinear())
    {
        delete p;
        return;
    }

    //! HERE: We have a valid point, and we can proceed

    m_canvas.clear();
    m_pts.Add(p);
    m_tris.erase(m_tris.begin() + i_enclosing);

    for (int i = 0; i < 3; i++)
    {
        bool did_flip = false;

        if (!IsDelaunay(tris_new[i].to_C2D(), m_pts))
        {
            C2DPoint *p1 = tris_new[i].v1;
            C2DPoint *p2 = tris_new[i].v2;
            C2DPoint *p3 = tris_new[i].v3;

            unsigned tri_adjacent_index = 0;
            C2DPoint *v_opposite = NULL;
            bool adj_exists = FindAdjacentTriangle(m_tris, p2, p3, &tri_adjacent_index, &v_opposite);

            if (adj_exists)
            {
                Tri tri_flip_1(p1, p2, v_opposite);
                Tri tri_flip_2(p1, p3, v_opposite);
                m_tris.erase(m_tris.begin() + tri_adjacent_index);
                m_tris.push_back(tri_flip_1);
                m_tris.push_back(tri_flip_2);
                m_canvas.add(new Triangle2D(tri_flip_1.to_vvr(vvr::green, true)));
                m_canvas.add(new Triangle2D(tri_flip_2.to_vvr(vvr::yellow, true)));
                did_flip = true;
            }
        }

        if (!did_flip)
        {
            m_canvas.add(new Triangle2D(tris_new[i].to_vvr(vvr::darkGreen, true)));
            m_tris.push_back(tris_new[i]);
        }
    }

    //! Visualize the violations.
    vector<unsigned> violations;
    FindViolations(m_tris, m_pts, violations);
    ShowViolations(m_tris, violations, m_canvas, vvr::magenta);
}

C2DCircle GetCircumCircle(const C2DTriangle &t)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Kataskeyaste ton kyklo pou "pernaei" apo tis 3 koryfes tou trigwnou.
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    C2DCircle circle;
    circle.SetCircumscribed(
        t.GetPoint1(),
        t.GetPoint2(),
        t.GetPoint3()
        );
    return circle;
}

bool IsDelaunay(const C2DTriangle &t, const C2DPointSet &pset)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Check if this triangle is a Delaunay triangle.
    //!
    //! HINTS:
    //!
    //!  - Shrink the circle a bit in order to exclude points of its circumference.
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    for (int i = 0; i < pset.size(); i++) {
        C2DCircle c = GetCircumCircle(t);
        c.SetRadius(c.GetRadius()*0.99);
        if (c.Contains(*pset.GetAt(i))) {
            return false;
        }
    }

    return true;
}

void FindViolations(vector<Tri> &tris, const C2DPointSet &ptset, vector<unsigned> &violations)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Check in `tris` for Delaunay violations.
    //!
    //! HINTS:
    //!
    //!  - If triangle i causes violation add it to violations vector like this:
    //!      violations.push_back(i);
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    violations.clear();

    for (int i = 0; i < tris.size(); i++)
    {
        Tri &tri = tris[i];
        C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
        if (!IsDelaunay(t, ptset)) {
            violations.push_back(i);
        }
    }
}

void ShowViolations(vector<Tri> &tris, const vector<unsigned> &violations, Canvas &canvas, const Colour &col)
{
    for (int i = 0; i < violations.size(); i++) {
        Tri &tri = tris[violations[i]];
        C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
        canvas.add(GetCircumCircle(t), col, false);
    }
}

bool FindAdjacentTriangle(vector<Tri> &tris, C2DPoint *p1, C2DPoint *p2, unsigned *tri_adj_index, C2DPoint **opp_ver)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Vreite ena trigwno pou exei koini plevra me ton
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    for (int i = 0; i < tris.size(); i++)
    {
        C2DPoint  *v1 = tris[i].v1;
        C2DPoint  *v2 = tris[i].v2;
        C2DPoint  *v3 = tris[i].v3;

        if (v1 == p1 && v2 == p2) { *opp_ver = v3; *tri_adj_index = i; return true; }
        if (v1 == p2 && v2 == p1) { *opp_ver = v3; *tri_adj_index = i; return true; }
        if (v2 == p1 && v3 == p2) { *opp_ver = v1; *tri_adj_index = i; return true; }
        if (v2 == p2 && v3 == p1) { *opp_ver = v1; *tri_adj_index = i; return true; }
        if (v3 == p1 && v1 == p2) { *opp_ver = v2; *tri_adj_index = i; return true; }
        if (v3 == p2 && v1 == p1) { *opp_ver = v2; *tri_adj_index = i; return true; }
    }

    return false;
}
