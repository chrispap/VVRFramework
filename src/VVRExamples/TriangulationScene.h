#ifndef SCENE_INTERSECTIONS_H
#define SCENE_INTERSECTIONS_H

#include <vector>
#include <scene.h>
#include <canvas.h>
#include <GeoLib.h>

#define APP_TITLE "VVR GeoLab - Triangulation"

using namespace std;
using namespace vvr;

struct Tri {
    C2DPoint *v1;
    C2DPoint *v2;
    C2DPoint *v3;

    Tri() : v1(NULL), v2(NULL), v3(NULL) {}
    Tri(C2DPoint *v1, C2DPoint *v2, C2DPoint *v3) : v1(v1), v2(v2), v3(v3) {}
};

C2DCircle GetCircumCircle(C2DTriangle &t);
bool IsDelaunay(C2DTriangle &t, C2DPointSet &pset);
bool FindAdjacentTriangle(vector<Tri> &tris, C2DPoint *p1, C2DPoint *p2, Tri **tri_adj, C2DPoint **opposite_vertex);

class TriangulationScene : public vvr::Scene
{
public:
    TriangulationScene();
    const char* getName() const override { return APP_TITLE; }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void arrowEvent(ArrowDir dir, int modif) override;

private:
    C2DTriangle make_tri_C2D(Tri &tri);
    Triangle2D  make_tri_2D(Tri &tri, Colour col=Colour::black);
    void handleNewPoint(C2DPoint *p);

private:
    Canvas2D        m_canvas_tris;
    Canvas2D        m_canvas_circles;
    C2DPointSet     m_pts;
    vector<Tri>     m_tris;
    Tri             m_tri_new[3];
};

#endif
