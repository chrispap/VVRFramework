#ifndef SCENE_TRIANGULATION_H
#define SCENE_TRIANGULATION_H

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/utils.h>
#include <GeoLib.h>
#include <vector>
#include <set>

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
    bool idle() override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;

private:
    void processPoint(C2DPoint* const p);

private:
    C2DPointSet m_pts;
    std::set<Tri> m_tris;
    vvr::Canvas m_canvas;
    int m_style_flag;
    float m_lw_canvas;
    float m_lw_tris;
    float m_sz_pt;
    bool m_anim_on;
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
    }
};

C2DCircle GetCircumCircle(const C2DTriangle &t);

bool IsDelaunay(const C2DTriangle &t, const C2DPointSet &pset);

void FindAdjacentTriangle(std::set<Tri> &tris, C2DPoint *p1, C2DPoint *p2, C2DPoint **opposite_vertex, bool erase);

void FixViolation(std::set<Tri> &tris, unsigned violation_index);

void ShowViolations(std::set<Tri> &tris, const C2DPointSet &ptset, vvr::Canvas &canvas, const vvr::Colour &col);

#endif // SCENE_TRIANGULATION_H
#include <algorithm>
#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include <MathGeoLib.h>

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
    C2DPoint* D = new C2DPoint(+W / 2, -H/2);
    
    m_pts.Add(A);
    m_pts.Add(B);
    m_pts.Add(C);
    m_pts.Add(D);
    m_tris.insert(Tri(A, B, C));
    m_tris.insert(Tri(A, C, D));

    m_anim_on = false;
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
    const bool ctrl_down = ctrlDown(modif);
    const bool alt_down = altDown(modif);
    const bool shift_down = shiftDown(modif);
    key = tolower(key);

    switch (key)
    {
    case ' ': m_anim_on^=true; break;
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

bool TriangulationScene::idle()
{
    if (!m_anim_on) return false;

    for (size_t rep = 0; rep < 1; rep++)
    {
        const unsigned N = std::min((size_t)100, m_tris.size());
        C2DPoint **pts = new C2DPoint*[N];
        int i = 0;
        set<Tri>::reverse_iterator tri_iter;
        for (tri_iter = m_tris.rbegin(); tri_iter != m_tris.rend(); ++tri_iter) {
            pts[i] = new C2DPoint(tri_iter->to_C2D().GetInCentre());
            if (++i == N) break;
        }
        for (size_t i = 0; i < N; i++) {
            unsigned size_before = m_pts.size();
            processPoint(pts[i]);
            if (m_pts.size() != size_before) break;
        }
        delete[] pts;
    }

    return true;
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

    set<Tri>::const_iterator tri_iter;
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
    //! [CHECK 1]
    //! Check whether a point already exists in the same coords.

    for (size_t i = 0; i < m_pts.size(); i++) {
        if (*m_pts.GetAt(i) == *p) {
            delete p; return;
        }
    }

    //! Find enclosing triangle.
    unsigned count_enclosing = 0;
    set<Tri>::iterator tri_iter, tri_enclosing_iter;
    for (tri_iter = m_tris.begin(); tri_iter != m_tris.end(); ++tri_iter) {
        if (tri_iter->to_C2D().Contains(*p)) {
            count_enclosing++;
            tri_enclosing_iter = tri_iter;
        }
    }

    //! [CHECK 2]
    //! If no enclosing triangle was found.
    //! Or if more than one were found.

    if (count_enclosing != 1) {
        delete p; return;
    }

    vector<Tri> tris_new;
    Tri tri_enclosing = *tri_enclosing_iter;
    tris_new.push_back(Tri(p, tri_enclosing.v1, tri_enclosing.v2));
    tris_new.push_back(Tri(p, tri_enclosing.v2, tri_enclosing.v3));
    tris_new.push_back(Tri(p, tri_enclosing.v3, tri_enclosing.v1));

    //! [CHECK 3]
    //! Check if any of the 3 triangles are colinear. (Use GeoLib's function)

    if (tris_new[0].to_C2D().Collinear() ||
        tris_new[1].to_C2D().Collinear() ||
        tris_new[2].to_C2D().Collinear())
    {
        delete p; return;
    }

    //! Here we have a valid point.
    //! We keep it and proceed.

    m_pts.Add(p);
    m_tris.erase(tri_enclosing_iter);
    m_canvas.clear();

    for (int i = 0; i < 3; i++)
    {
        if (!IsDelaunay(tris_new[i].to_C2D(), m_pts))
        {
            C2DPoint *p1 = tris_new[i].v1;
            C2DPoint *p2 = tris_new[i].v2;
            C2DPoint *p3 = tris_new[i].v3;
            C2DPoint *v_opposite = NULL;
            unsigned tri_adjacent_index;
            FindAdjacentTriangle(m_tris, p2, p3, &v_opposite, true);
            Tri tri_flip_1(p1, p2, v_opposite);
            Tri tri_flip_2(p1, p3, v_opposite);
            m_tris.insert(tri_flip_1);
            m_tris.insert(tri_flip_2);
            m_canvas.add(new Triangle2D(tri_flip_1.to_vvr(vvr::green, true)));
            m_canvas.add(new Triangle2D(tri_flip_2.to_vvr(vvr::yellow, true)));
        }
        else
        {
            m_canvas.add(new Triangle2D(tris_new[i].to_vvr(vvr::darkGreen, true)));
            m_tris.insert(tris_new[i]);
        }
    }

    //! Find violations. Fix them. Visualize.
    ShowViolations(m_tris, m_pts, m_canvas, vvr::magenta);
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
        c.SetRadius(c.GetRadius()*0.95);
        if (c.Contains(*pset.GetAt(i))) {
            return false;
        }
    }

    return true;
}

void FindAdjacentTriangle(set<Tri> &tris, C2DPoint *p1, C2DPoint *p2, C2DPoint **opp_ver, bool erase)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Vreite ena trigwno pou exei koini plevra me ton
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    for (set<Tri>::const_iterator tri_iter = tris.begin(); tri_iter != tris.end(); ++tri_iter)
    {
        C2DPoint  *v1 = tri_iter->v1;
        C2DPoint  *v2 = tri_iter->v2;
        C2DPoint  *v3 = tri_iter->v3;

        if (v1 == p1 && v2 == p2) { *opp_ver = v3; tris.erase(tri_iter); return; }
        if (v1 == p2 && v2 == p1) { *opp_ver = v3; tris.erase(tri_iter); return; }
        if (v2 == p1 && v3 == p2) { *opp_ver = v1; tris.erase(tri_iter); return; }
        if (v2 == p2 && v3 == p1) { *opp_ver = v1; tris.erase(tri_iter); return; }
        if (v3 == p1 && v1 == p2) { *opp_ver = v2; tris.erase(tri_iter); return; }
        if (v3 == p2 && v1 == p1) { *opp_ver = v2; tris.erase(tri_iter); return; }
    }
}

void ShowViolations(set<Tri> &tris, const C2DPointSet &ptset, Canvas &canvas, const Colour &col)
{
    set<Tri>::const_iterator tri_iter;
    for (tri_iter = tris.begin(); tri_iter != tris.end(); ++tri_iter) {
        const Tri &tri = *tri_iter;
        C2DTriangle t = tri_iter->to_C2D();
        if (!IsDelaunay(t, ptset)) {
            canvas.add(GetCircumCircle(t), col, false);
            canvas.add(new Triangle2D(tri.to_vvr(col, true)));
        }
    }
}

void FixViolation(set<Tri> &tris, unsigned violation_index)
{

}
