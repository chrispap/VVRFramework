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
void FindViolations(vector<Tri> &tris, C2DPointSet &ptset, vector<Tri> &tris_violating);
void FixViolations(vector<Tri> &tris, C2DPointSet &ptset);
void ShowViolations(vector<Tri> &tris, Canvas2D &canvas, Colour &col);

class TriangulationScene : public vvr::Scene
{
public:
    TriangulationScene();
    const char* getName() const override { return APP_TITLE; }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void arrowEvent(ArrowDir dir, int modif) override;

private:
    void handleNewPoint(C2DPoint *p);
    C2DTriangle make_tri_C2D(const Tri &tri) { return C2DTriangle(*tri.v1, *tri.v2, *tri.v3); }
    Triangle2D  make_tri_2D(const Tri &tri, Colour col = Colour::black) {
        return Triangle2D(tri.v1->x, tri.v1->y, tri.v2->x, tri.v2->y, tri.v3->x, tri.v3->y, col);
    }

private:
    Canvas2D        m_canvas;
    C2DPointSet     m_pts;
    vector<Tri>     m_tris;
    int             m_show_step;
};

#endif
