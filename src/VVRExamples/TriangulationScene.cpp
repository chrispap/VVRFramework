#include "TriangulationScene.h"
#include <utils.h>
#include <iostream>
#include <string>
#include <ctime>
#include <algorithm>

TriangulationScene::TriangulationScene()
{
    m_bg_col = Colour(0x44, 0x44, 0x44);
    reset();
}

void TriangulationScene::reset()
{
    Scene::reset();

    m_canvas_tris.clear();
    m_canvas_circles.clear();

    m_tris.clear();
    m_pts.DeleteAll();
    m_pts.Add(new C2DPoint (-350, -200));
    m_pts.Add(new C2DPoint ( 350, -200));
    m_pts.Add(new C2DPoint (   0,  300));
    m_tris.push_back(Tri(m_pts.GetAt(0), m_pts.GetAt(1), m_pts.GetAt(2)));
}

void TriangulationScene::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);
    handleNewPoint(new C2DPoint(x,y));
}

void TriangulationScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::LEFT) {
        if (m_canvas_tris.isAtStart()) {
            /*m_canvas_tris.ff();*/ 
        }
        else {
            m_canvas_tris.prev();
            m_canvas_circles.prev();
        }
    }
    else if (dir == vvr::RIGHT) {
        if (m_canvas_tris.isAtEnd()) { 
            /*m_canvas_tris.rew();*/ 
        }
        else {
            m_canvas_tris.next();
            m_canvas_circles.next();
        }
    }
}

void TriangulationScene::handleNewPoint(C2DPoint *p)
{
    m_pts.Add(p);

    // Find enclosing triangle.
    int i_encl;
    for (i_encl = 0; i_encl < m_tris.size(); i_encl++) {
        if (make_tri_C2D(m_tris[i_encl]).Contains(*p)) break;
    }

    if (i_encl == m_tris.size()) return;
    
    Tri tri_encl = m_tris[i_encl];

    // Remove the enclosing triangle. It will be replaced by its subdivision triangles.
    m_tris.erase(m_tris.begin() + i_encl);

    // Create 3 new triangles
    m_tri_new[0] = Tri(p, tri_encl.v1, tri_encl.v2);
    m_tri_new[1] = Tri(p, tri_encl.v2, tri_encl.v3);
    m_tri_new[2] = Tri(p, tri_encl.v3, tri_encl.v1);

    // Create new frames in canvas objects.
    m_canvas_tris.newFrame(false); m_canvas_circles.newFrame(false);

    for (int i = 0; i < 3; i++) {
        m_canvas_tris.add(make_tri_C2D(m_tri_new[i]), Colour(255, 50, 50), true);
        m_canvas_tris.add(make_tri_C2D(m_tri_new[i]), Colour::black, false);
    }

    vector<Tri> tris_adj;

    // Chech new triangles for Delaunay condition
    for (int i = 0; i < 3; i++)
    {
        C2DTriangle tri_check = make_tri_C2D(m_tri_new[i]);
        bool is_Del = IsDelaunay(tri_check, m_pts);
        C2DPoint *v_opposite;
        Tri *tri_adjacent;

        if ( !is_Del ) 
        {
            m_canvas_circles.add(GetCircumCircle(tri_check), Colour::magenta, false);

            C2DPoint *v2 = m_tri_new[i].v2;
            C2DPoint *v3 = m_tri_new[i].v3;

            FindAdjacentTriangle(m_tris, v2, v3, &tri_adjacent, &v_opposite);
            m_canvas_tris.add(make_tri_C2D(*tri_adjacent), Colour::darkOrange, true);
            m_canvas_tris.add(make_tri_C2D(*tri_adjacent), Colour::black, false);

            // Flip triangle
            m_tri_new[i].v1 = p;
            m_tri_new[i].v2 = v2;
            m_tri_new[i].v3 = v_opposite;
            
            // Flip adjacent triangle
            tri_adjacent->v1 = p;
            tri_adjacent->v2 = v3;
            tri_adjacent->v3 = v_opposite;

            tris_adj.push_back(*tri_adjacent); // Keep in order to add to canvas later.
        }

    }

    unsigned n = tris_adj.size();
    m_canvas_tris.newFrame(false); m_canvas_circles.newFrame(false);
    for (int i = 0; i < 3; i++) m_canvas_tris.add(make_tri_C2D(m_tri_new[i]), Colour::green, true);
    for (int i = 0; i < n; i++) m_canvas_tris.add(make_tri_C2D(tris_adj[i]),  Colour::green, true);
    for (int i = 0; i < 3; i++) m_canvas_tris.add(make_tri_C2D(m_tri_new[i]), Colour::black, false);
    for (int i = 0; i < n; i++) m_canvas_tris.add(make_tri_C2D(tris_adj[i]),  Colour::black, false);
    m_canvas_tris.prev(); m_canvas_circles.prev();

    // Add the new triangles.
    m_tris.push_back(m_tri_new[0]);
    m_tris.push_back(m_tri_new[1]);
    m_tris.push_back(m_tri_new[2]);
}

void TriangulationScene::draw()
{
    enterPixelMode();
    
    for (int i = 0; i < m_tris.size(); i++)             // Draw triangles
        make_tri_2D(m_tris[i], Colour::cyan).draw();
    m_canvas_tris.draw();                               // Draw current step triangles
    vvr::draw(m_pts, Colour::red);                      // Draw points
    m_canvas_circles.draw();                            // Draw violation circles
}

C2DTriangle TriangulationScene::make_tri_C2D(Tri &tri)
{
    return C2DTriangle(*tri.v1, *tri.v2, *tri.v3);
}

Triangle2D TriangulationScene::make_tri_2D(Tri &tri, Colour col)
{
    return Triangle2D(
        tri.v1->x, tri.v1->y,
        tri.v2->x, tri.v2->y,
        tri.v3->x, tri.v3->y, col);
}

C2DCircle GetCircumCircle(C2DTriangle &t)
{
    C2DCircle circle;

    /////////////////////////////////////////////////////////////////////////////////////
    // Kataskeyaste ton kyklo pou "pernaei" apo tis 3 koryfes tou trigwnou. /////////////
    /////////////////////////////////////////////////////////////////////////////////////

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

    /////////////////////////////////////////////////////////////////////////////////////
    // Check with all points ////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////

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

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new TriangulationScene);
}
