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
    vvr::Canvas2D m_canvas;
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

    vvr::Triangle2D to_vvr(vvr::Colour col = vvr::Colour::black, bool filled = false) const {
        vvr::Triangle2D t(v1->x, v1->y, v2->x, v2->y, v3->x, v3->y, col);
        t.setSolidRender(filled);
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

void ShowViolations(std::set<Tri> &tris, const C2DPointSet &ptset, vvr::Canvas2D &canvas, const vvr::Colour &col);

#endif // SCENE_TRIANGULATION_H
