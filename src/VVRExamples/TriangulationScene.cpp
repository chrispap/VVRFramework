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

void TriangulationScene::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);
    if (m_pts.GetLast()->Distance(C2DPoint(x, y)) > 20)
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
    // Check whether a point already exists in the same coords.
    for (size_t i = 0; i < m_pts.size(); i++)
    {
        if (m_pts.GetAt(i)->x == p->x &&
            m_pts.GetAt(i)->y == p->y)
        {
            delete p;
            return;
        }
    }

    // Find enclosing triangle.
    int i_encl;
    for (i_encl = 0; i_encl < m_tris.size(); i_encl++) {
        if (make_tri_C2D(m_tris[i_encl]).Contains(*p))
            break;
    }

    // If no enclosing triangle was found.
    if (i_encl == m_tris.size()) {
        delete p;
        return;
    }

    Tri encl_tri = m_tris[i_encl];
    m_pts.Add(p);
    m_canvas.clear();

    // Remove the enclosing triangle. It will be replaced by its subdivision triangles.
    m_tris.erase(m_tris.begin() + i_encl);

    // Create the 3 subdivision triangles.
    Tri new_tris[3];
    new_tris[0] = Tri(p, encl_tri.v1, encl_tri.v2);
    new_tris[1] = Tri(p, encl_tri.v2, encl_tri.v3);
    new_tris[2] = Tri(p, encl_tri.v3, encl_tri.v1);

    // Check if any of the 3 triangles are colinear.
    for (int i = 0; i < 3; i++) {
        if (make_tri_C2D(new_tris[i]).Collinear())
            return;
    }

    // Display the 3 the triangles.
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
            Tri nt1 = new_tris[i];
            Tri nt2 = *tri_adjacent;

            nt1.v1 = p;
            nt1.v2 = v2;
            nt1.v3 = v_opposite;
            nt2.v1 = p;
            nt2.v2 = v3;
            nt2.v3 = v_opposite;

            if (make_tri_C2D(nt1).Collinear() ||
                make_tri_C2D(nt2).Collinear()) {
                continue;
            }

            new_tris[i] = nt1;
            *tri_adjacent = nt2;

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

    vector<unsigned> violations;
    FixViolations(m_tris, m_pts);
    FindViolations(m_tris, m_pts, violations);
    ShowViolations(m_tris, violations, m_canvas, Colour::magenta);

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

void FindViolations(vector<Tri> &tris, C2DPointSet &ptset, vector<unsigned> &violations)
{
    for (int i = 0; i < tris.size(); i++)
    {
        Tri &tri = tris[i];
        C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
        if (!IsDelaunay(t, ptset)) {
            violations.push_back(i);
        }
    }
}

void ShowViolations(vector<Tri> &tris, vector<unsigned> &violations, Canvas2D &canvas, Colour &col)
{
    canvas.newFrame(false);
    for (int i = 0; i < violations.size(); i++) {
        Tri &tri = tris[violations[i]];
        C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
        canvas.add(GetCircumCircle(t), col, false);
    }
}

void FixViolations(vector<Tri> &tris, C2DPointSet &ptset)
{
    vector<unsigned> violations;

    FindViolations(tris, ptset, violations);

    int attempts = 100;

    while (!violations.empty() && attempts>0)
    {
        int num_viol = violations.size();

        for (int i = 0; i < violations.size(); i++)
        {
            Tri &tri = tris[violations[i]];
            C2DTriangle t(*tri.v1, *tri.v2, *tri.v3);
            bool is_Del = IsDelaunay(t, ptset);
            C2DPoint *v_opposite;
            Tri *tri_adjacent;

            if (!is_Del)
            {
                C2DPoint *v1 = tri.v1;
                C2DPoint *v2 = tri.v2;
                C2DPoint *v3 = tri.v3;

                if (!FindAdjacentTriangle(tris, v2, v3, &tri_adjacent, &v_opposite)) {
                    C2DPoint *tmp = v2;
                    v2 = v1;
                    v1 = tmp;
                    if (!FindAdjacentTriangle(tris, v2, v3, &tri_adjacent, &v_opposite))
                        continue;
                }

                if (tri_adjacent == &tri) continue;

                // Flip triangle
                Tri nt1 = tri;
                Tri nt2 = *tri_adjacent;

                nt1.v1 = v1;
                nt1.v2 = v2;
                nt1.v3 = v_opposite;
                nt2.v1 = v1;
                nt2.v2 = v3;
                nt2.v3 = v_opposite;

                if (C2DTriangle(*nt1.v1, *nt1.v2, *nt1.v3).Collinear() ||
                    C2DTriangle(*nt2.v1, *nt2.v2, *nt2.v3).Collinear()) {
                    continue;
                }

                tri = nt1;
                *tri_adjacent = nt2;

                break;
            }
        }

        violations.clear();
        FindViolations(tris, ptset, violations);

        if (num_viol == violations.size())
            --attempts;
        else {
            echo(attempts);
            attempts = 100;
        }

    }

}

//! Main 

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new TriangulationScene);
}
