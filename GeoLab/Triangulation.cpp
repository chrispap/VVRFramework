#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/utils.h>
#include <GeoLib.h>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <string>
#include <ctime>

/*---[Definitions]----------------------------------------------------------------------*/
struct Tri;

class DelaunayScene : public vvr::Scene
{
public:
    DelaunayScene();

    const char* getName() const override {
        return "Delaunay (Manual)";
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

class DelaunayAutoScene : public vvr::Scene
{
public:
    DelaunayAutoScene();

    const char* getName() const override {
        return "Delaunay (Auto)";
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

/*--------------------------------------------------------------------------------------*/
C2DCircle GetCircumCircle(const C2DTriangle &t)
{
    C2DCircle circle;
    circle.SetCircumscribed(t.GetPoint1(), t.GetPoint2(), t.GetPoint3());
    return circle;
}

bool IsDelaunay(const C2DTriangle &t, const C2DPointSet &pset)
{
    for (int i = 0; i < pset.size(); i++) {
        C2DCircle c = GetCircumCircle(t);
        c.SetRadius(c.GetRadius()*0.99);
        if (c.Contains(*pset.GetAt(i))) {
            return false;
        }
    }

    return true;
}

/*--------------------------------------------------------------------------------------*/
void FindViolations(std::vector<Tri> &tris, const C2DPointSet &ptset, std::vector<unsigned> &violations)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Check in `tris` for Delaunay violations.
    //!
    //! HINTS:
    //!
    //!  - If triangle i causes violation add it to violations std::vector like this:
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

void ShowViolations(std::vector<Tri> &tris, const std::vector<unsigned> &violations, vvr::Canvas &canvas, const vvr::Colour &col)
{
    for (int i = 0; i < violations.size(); i++) {
        Tri &tri = tris[violations[i]];
        C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
        canvas.add(GetCircumCircle(t), col, false);
    }
}

bool FindAdjacentTriangle(std::vector<Tri> &tris, C2DPoint *p1, C2DPoint *p2, unsigned *tri_adj_index, C2DPoint **opp_ver)
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

/*--------------------------------------------------------------------------------------*/
void FindAdjacentTriangle(std::set<Tri> &tris, C2DPoint *p1, C2DPoint *p2, C2DPoint **opp_ver, bool erase)
{
    //!//////////////////////////////////////////////////////////////////////////////////
    //! TASK:
    //!
    //!  - Vreite ena trigwno pou exei koini plevra me ton
    //!
    //!//////////////////////////////////////////////////////////////////////////////////

    for (std::set<Tri>::const_iterator tri_iter = tris.begin(); tri_iter != tris.end(); ++tri_iter)
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

void ShowViolations(std::set<Tri> &tris, const C2DPointSet &ptset, vvr::Canvas &canvas, const vvr::Colour &col)
{
    std::set<Tri>::const_iterator tri_iter;
    for (tri_iter = tris.begin(); tri_iter != tris.end(); ++tri_iter) {
        const Tri &tri = *tri_iter;
        C2DTriangle t = tri_iter->to_C2D();
        if (!IsDelaunay(t, ptset)) {
            canvas.add(GetCircumCircle(t), col, false);
            canvas.add(new vvr::Triangle2D(tri.to_vvr(col, true)));
        }
    }
}

void FixViolation(std::set<Tri> &tris, unsigned violation_index)
{

}

/*---[TriangulationScene]---------------------------------------------------------------*/
DelaunayScene::DelaunayScene()
{
    m_bg_col = vvr::Colour(0x44, 0x44, 0x44);
    m_create_menus = false;
    m_lw_canvas = 2;
    m_lw_tris = 3;
    m_sz_pt = 10;
}

void DelaunayScene::reset()
{
    Scene::reset();
    m_style_flag = 0;
    m_style_flag |= 1;
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

void DelaunayScene::resize()
{
    static bool first_pass = true;
    if (first_pass) reset();
    first_pass = false;
}

void DelaunayScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    processPoint(new C2DPoint(x, y));
}

void DelaunayScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    if (m_pts.GetLast()->Distance(C2DPoint(x, y)) > 80)
        processPoint(new C2DPoint(x, y));
}

void DelaunayScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
    case 'c': m_style_flag ^= 1; break;
    }
}

void DelaunayScene::mouseWheel(int dir, int modif)
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

void DelaunayScene::draw()
{
    enterPixelMode();

    //! Draw violations and anything else added to canvas
    if (m_style_flag & 1) {
        vvr::Shape::LineWidth = m_lw_canvas;
        m_canvas.draw();
    }

    //! Draw triangles
    vvr::Shape::LineWidth = m_lw_tris;

    std::vector<Tri>::const_iterator tri_iter;
    for (tri_iter = m_tris.begin(); tri_iter != m_tris.end(); ++tri_iter) {
        tri_iter->to_vvr(vvr::black).draw();
    }

    //! Draw points
    vvr::Shape::PointSize = m_sz_pt;
    vvr::draw(m_pts, vvr::red);

    exitPixelMode();
}

/*---[TriangulationExpScene]------------------------------------------------------------*/
DelaunayAutoScene::DelaunayAutoScene()
{
    m_bg_col = vvr::Colour(0x44, 0x44, 0x44);
    m_create_menus = false;
    m_lw_canvas = 2;
    m_lw_tris = 3;
    m_sz_pt = 10;
}

void DelaunayAutoScene::reset()
{
    Scene::reset();
    m_style_flag = 0;
    m_style_flag |= 1;
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

void DelaunayAutoScene::resize()
{
    static bool first_pass = true;
    if (first_pass) reset();
    first_pass = false;
}

void DelaunayAutoScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    processPoint(new C2DPoint(x, y));
}

void DelaunayAutoScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    if (m_pts.GetLast()->Distance(C2DPoint(x, y)) > 80)
        processPoint(new C2DPoint(x, y));
}

void DelaunayAutoScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
    case ' ': m_anim_on^=true; break;
    case 'c': m_style_flag ^= 1; break;
    }
}

void DelaunayAutoScene::mouseWheel(int dir, int modif)
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

bool DelaunayAutoScene::idle()
{
    if (!m_anim_on) return false;

    for (size_t rep = 0; rep < 1; rep++)
    {
        const unsigned N = std::min((size_t)100, m_tris.size());
        C2DPoint **pts = new C2DPoint*[N];
        int i = 0;
        std::set<Tri>::reverse_iterator tri_iter;
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

void DelaunayAutoScene::draw()
{
    enterPixelMode();

    //! Draw violations and anything else added to canvas
    if (m_style_flag & 1) {
        vvr::Shape::LineWidth = m_lw_canvas;
        m_canvas.draw();
    }

    //! Draw triangles
    vvr::Shape::LineWidth = m_lw_tris;

    std::set<Tri>::const_iterator tri_iter;
    for (tri_iter = m_tris.begin(); tri_iter != m_tris.end(); ++tri_iter) {
        tri_iter->to_vvr(vvr::black).draw();
    }

    //! Draw points
    vvr::Shape::PointSize = m_sz_pt;
    vvr::draw(m_pts, vvr::red);

    exitPixelMode();
}

void DelaunayAutoScene::processPoint(C2DPoint* const p)
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
    std::set<Tri>::iterator tri_iter, tri_enclosing_iter;
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

    std::vector<Tri> tris_new;
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
            FindAdjacentTriangle(m_tris, p2, p3, &v_opposite, true);
            Tri tri_flip_1(p1, p2, v_opposite);
            Tri tri_flip_2(p1, p3, v_opposite);
            m_tris.insert(tri_flip_1);
            m_tris.insert(tri_flip_2);
            m_canvas.add(new vvr::Triangle2D(tri_flip_1.to_vvr(vvr::green, true)));
            m_canvas.add(new vvr::Triangle2D(tri_flip_2.to_vvr(vvr::yellow, true)));
        }
        else
        {
            m_canvas.add(new vvr::Triangle2D(tris_new[i].to_vvr(vvr::darkGreen, true)));
            m_tris.insert(tris_new[i]);
        }
    }

    //! Find violations. Fix them. Visualize.
    ShowViolations(m_tris, m_pts, m_canvas, vvr::magenta);
}

void DelaunayScene::processPoint(C2DPoint* const p)
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

    std::vector<Tri> tris_new;
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
                m_canvas.add(new vvr::Triangle2D(tri_flip_1.to_vvr(vvr::green, true)));
                m_canvas.add(new vvr::Triangle2D(tri_flip_2.to_vvr(vvr::yellow, true)));
                did_flip = true;
            }
        }

        if (!did_flip)
        {
            m_canvas.add(new vvr::Triangle2D(tris_new[i].to_vvr(vvr::darkGreen, true)));
            m_tris.push_back(tris_new[i]);
        }
    }

    //! Visualize the violations.
    std::vector<unsigned> violations;
    FindViolations(m_tris, m_pts, violations);
    ShowViolations(m_tris, violations, m_canvas, vvr::magenta);
}

/*---[Invoke]---------------------------------------------------------------------------*/
#ifndef ALL_DEMO_APP
vvr_invoke_main_with_scene(DelaunayScene)
#endif
