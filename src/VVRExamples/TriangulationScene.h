#ifndef SCENE_INTERSECTIONS_H
#define SCENE_INTERSECTIONS_H

#include <vector>
#include <scene.h>
#include <canvas.h>
#include <GeoLib.h>

#define APP_TITLE "VVR GeoLab - Triangulation"

using namespace std;
using namespace vvr;

//! Struct representing a triangle with its 3 vertices
struct Tri {
    C2DPoint *v1;
    C2DPoint *v2;
    C2DPoint *v3;

    Tri() : v1(NULL), v2(NULL), v3(NULL) {}

    Tri(C2DPoint *v1, C2DPoint *v2, C2DPoint *v3) : v1(v1), v2(v2), v3(v3) {}

    C2DTriangle to_C2D() const { return C2DTriangle(*v1, *v2, *v3); }

    Triangle2D to_2D(Colour col = Colour::black) const { return Triangle2D(
                    v1->x, v1->y,
                    v2->x, v2->y,
                    v3->x, v3->y, col);
    }
};

C2DCircle GetCircumCircle(const C2DTriangle &t);
bool IsDelaunay(const C2DTriangle &t, const C2DPointSet &pset);
bool FindAdjacentTriangle(vector<Tri> &tris, C2DPoint *p1, C2DPoint *p2, Tri **tri_adj, C2DPoint **opposite_vertex);
void FindViolations(vector<Tri> &tris, const C2DPointSet &ptset, vector<unsigned> &violations);
void ShowViolations(vector<Tri> &tris, const vector<unsigned> &violations, Canvas2D &canvas, Colour &col);
void FixViolations(vector<Tri> &tris, const C2DPointSet &ptset);

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

private:
    Canvas2D        m_canvas;
    C2DPointSet     m_pts;
    vector<Tri>     m_tris;
    int             m_show_step;
};

#endif
