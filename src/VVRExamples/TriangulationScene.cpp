#include "TriangulationScene.h"
#include <utils.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <ctime>

TriangulationScene::TriangulationScene()
{
    m_bg_col = Colour(0x44, 0x44, 0x44);
    m_show_step = 0;
    reset();
}

void TriangulationScene::reset()
{
    Scene::reset();

    m_canvas.clear();
    m_tris.clear();
    m_pts.DeleteAll();
    m_pts.Add(new C2DPoint(-350, -200));
    m_pts.Add(new C2DPoint(350, -200));
    m_pts.Add(new C2DPoint(0, 300));
    m_tris.push_back(Tri(m_pts.GetAt(0), m_pts.GetAt(1), m_pts.GetAt(2)));
}

void TriangulationScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    handleNewPoint(new C2DPoint(x, y));
}

void TriangulationScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::LEFT)
    {
        if (m_show_step > 0)
            --m_show_step;

        if (m_canvas.isAtStart()) {

        }
        else {
            m_canvas.prev();
        }
    }
    else if (dir == vvr::RIGHT)
    {
        if (m_show_step < 2)
            ++m_show_step;

        if (m_canvas.isAtEnd()) {

        }
        else {
            m_canvas.next();
        }
    }
}

void TriangulationScene::handleNewPoint(C2DPoint *p)
{
    // Check whether a same point already exists.
    for (size_t i = 0; i < m_pts.size(); i++)
    {
        if (m_pts.GetAt(i)->x == p->x &&
            m_pts.GetAt(i)->y == p->y) {
            return;
        }
    }

    // Find enclosing triangle.
    int i_encl;
    for (i_encl = 0; i_encl < m_tris.size(); i_encl++) {
        if (make_tri_C2D(m_tris[i_encl]).Contains(*p))
            break;
    }

    if (i_encl == m_tris.size()) // Did not find enclosing triangle.
        return;

    m_pts.Add(p);
    m_canvas.clear();

    Tri encl_tri = m_tris[i_encl];

    // Remove the enclosing triangle. It will be replaced by its subdivision triangles.
    m_tris.erase(m_tris.begin() + i_encl);

    // Create the 3 subdivision triangles.
    Tri new_tris[3];
    new_tris[0] = Tri(p, encl_tri.v1, encl_tri.v2);
    new_tris[1] = Tri(p, encl_tri.v2, encl_tri.v3);
    new_tris[2] = Tri(p, encl_tri.v3, encl_tri.v1);

    for (int i = 0; i < 3; i++) {
        if (make_tri_C2D(new_tris[i]).Collinear()) 
            return;
    }

    for (int i = 0; i < 3; i++) {
        m_canvas.add(make_tri_C2D(new_tris[i]), Colour(255, 50, 50), true);
        m_canvas.add(make_tri_C2D(new_tris[i]), Colour::black, false);
    }

    vector<Tri> tris_adj;

    // Chech the new triangles for Delaunay violations.
    for (int i = 0; i < 3; i++)
    {
        C2DTriangle tri_check = make_tri_C2D(new_tris[i]);
        bool is_Del = IsDelaunay(tri_check, m_pts);
        C2DPoint *v_opposite;
        Tri *tri_adjacent;

        if (!is_Del)
        {
            m_canvas.add(GetCircumCircle(tri_check), Colour::magenta, false);

            C2DPoint *v2 = new_tris[i].v2;
            C2DPoint *v3 = new_tris[i].v3;

            if (!FindAdjacentTriangle(m_tris, v2, v3, &tri_adjacent, &v_opposite))
                break;

            m_canvas.add(make_tri_C2D(*tri_adjacent), Colour::darkOrange, true);
            m_canvas.add(make_tri_C2D(*tri_adjacent), Colour::black, false);

            // Flip triangle
            new_tris[i].v1 = p;
            new_tris[i].v2 = v2;
            new_tris[i].v3 = v_opposite;

            // Flip adjacent triangle
            tri_adjacent->v1 = p;
            tri_adjacent->v2 = v3;
            tri_adjacent->v3 = v_opposite;

            tris_adj.push_back(*tri_adjacent); // Keep in order to add to canvas later.
        }

    }

    m_canvas.newFrame(false);
    unsigned n = tris_adj.size();
    for (int i = 0; i < 3; i++) m_canvas.add(make_tri_C2D(new_tris[i]), Colour::green, 1);
    for (int i = 0; i < n; i++) m_canvas.add(make_tri_C2D(tris_adj[i]), Colour::green, 1);
    for (int i = 0; i < 3; i++) m_canvas.add(make_tri_C2D(new_tris[i]), Colour::black, 0);
    for (int i = 0; i < n; i++) m_canvas.add(make_tri_C2D(tris_adj[i]), Colour::black, 0);

    // Add the new triangles.
    m_tris.push_back(new_tris[0]);
    m_tris.push_back(new_tris[1]);
    m_tris.push_back(new_tris[2]);

    vector<Tri> tris_violating;
    FindViolations(m_tris, m_pts, tris_violating);
    ShowViolations(tris_violating, m_canvas);
    FixViolations(tris_violating, m_pts);

    m_canvas.rew();
    for (int i = 0; i < m_show_step; i++) 
        m_canvas.next();
}

void TriangulationScene::draw()
{
    enterPixelMode();

    for (int i = 0; i < m_tris.size(); i++)
        make_tri_2D(m_tris[i], Colour::cyan).draw();    // Draw triangles
    m_canvas.draw();                                    // Draw current step violations
    vvr::draw(m_pts, Colour::red);                      // Draw points
}

//! Task Functions

C2DCircle GetCircumCircle(C2DTriangle &t)
{
    /////////////////////////////////////////////////////////////////////////////////////
    // Kataskeyaste ton kyklo pou "pernaei" apo tis 3 koryfes tou trigwnou. /////////////
    /////////////////////////////////////////////////////////////////////////////////////

    C2DCircle circle;
    circle.SetCentre(t.GetCircumCentre());
    circle.SetRadius(circle.GetCentre().Distance(t.GetPoint1()));
    return circle;
}

bool IsDelaunay(C2DTriangle &t, C2DPointSet &pset)
{
    bool isDel = true;

    // Get the circumcircle of the triangle and
    // slightly decrease its radius in order to
    // exclude points of the circumference.

    C2DCircle cc = GetCircumCircle(t);
    cc.SetRadius(cc.GetRadius() * 0.999);

    for (int i = 0; i < pset.size(); i++)
    {
        if (cc.Contains(*pset.GetAt(i))) {
            isDel = false;
            break;
        }
    }

    return isDel;
}

bool FindAdjacentTriangle(vector<Tri> &tris, C2DPoint *p1, C2DPoint *p2, Tri **tri_adj, C2DPoint **opp_ver)
{
    for (int i = 0; i < tris.size(); i++)
    {
        C2DPoint  *v1 = tris[i].v1;
        C2DPoint  *v2 = tris[i].v2;
        C2DPoint  *v3 = tris[i].v3;

        if (v1 == p1 && v2 == p2) { *opp_ver = v3; *tri_adj = &tris[i]; return true; }
        if (v1 == p2 && v2 == p1) { *opp_ver = v3; *tri_adj = &tris[i]; return true; }
        if (v2 == p1 && v3 == p2) { *opp_ver = v1; *tri_adj = &tris[i]; return true; }
        if (v2 == p2 && v3 == p1) { *opp_ver = v1; *tri_adj = &tris[i]; return true; }
        if (v3 == p1 && v1 == p2) { *opp_ver = v2; *tri_adj = &tris[i]; return true; }
        if (v3 == p2 && v1 == p1) { *opp_ver = v2; *tri_adj = &tris[i]; return true; }
    }

    return false;
}

void FindViolations(vector<Tri> &tris, C2DPointSet &ptset, vector<Tri> &tris_violating)
{
    for (int i = 0; i < tris.size(); i++)
    {
        Tri &tri = tris[i];
        C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
        if (!IsDelaunay(t, ptset)) {
            tris_violating.push_back(tris[i]);
        }
    }
}

void ShowViolations(vector<Tri> &tris_violating, Canvas2D &canvas)
{
    canvas.newFrame(false);
    for (int i = 0; i < tris_violating.size(); i++) {
        Tri &tri = tris_violating[i];
        C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
        canvas.add(GetCircumCircle(t), Colour::magenta, false);
    }
}

void FixViolations(vector<Tri> &tris, C2DPointSet &ptset)
{
    vector<Tri> tris_adj;
    vector<Tri> tris_violating;

    FindViolations(tris, ptset, tris_violating);

    while (tris_violating.empty() == false) {
        for (int i = 0; i < tris_violating.size(); i++)
        {
            Tri &tri = tris_violating[i];
            C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
            bool is_Del = IsDelaunay(t, ptset);
            C2DPoint *v_opposite;
            Tri *tri_adjacent;

            if (!is_Del)
            {
                C2DPoint *v1 = tris_violating[i].v1;
                C2DPoint *v2 = tris_violating[i].v2;
                C2DPoint *v3 = tris_violating[i].v3;

                if (!FindAdjacentTriangle(tris, v2, v3, &tri_adjacent, &v_opposite)) {
                    C2DPoint *tmp = v2;
                    v2 = v1;
                    v1 = tmp;
                    FindAdjacentTriangle(tris, v2, v3, &tri_adjacent, &v_opposite);
                }

                if (tri_adjacent == &tris_violating[i]) continue;

                // Flip triangle
                tris_violating[i].v1 = v1;
                tris_violating[i].v2 = v2;
                tris_violating[i].v3 = v_opposite;

                // Flip adjacent triangle
                tri_adjacent->v1 = v1;
                tri_adjacent->v2 = v3;
                tri_adjacent->v3 = v_opposite;

                tris_adj.push_back(*tri_adjacent); // Keep in order to add to canvas later.
                break;
            }
        }

        tris_violating.clear();
        FindViolations(tris, ptset, tris_violating);
    }

}

//! Main 

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new TriangulationScene);
}
