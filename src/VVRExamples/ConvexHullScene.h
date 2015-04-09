#ifndef SCENE_CONVEX_HULL_H
#define SCENE_CONVEX_HULL_H

#include <scene.h>
#include <canvas.h>
#include <GeoLib/GeoLib.h>

#define APP_TITLE "VVR GeoLab - Convex Hull"

using namespace std;
using namespace vvr;

class ConvexHullScene : public vvr::Scene
{
public:

    ConvexHullScene();
    const char* getName() const override { return APP_TITLE; }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(ArrowDir dir, int modif) override;

private:
    void ConvexHull_Slow();
    void ConvexHull_Fast();

private:
    static void createRandomPoints(C2DPointSet &point_set, int num_pts);
    static void draw(C2DPointSet &point_set, Colour &col);
    static void draw(C2DLineSet  &line_set,  Colour &col);
    static void draw(C2DPolygon  &polygon,   Colour &col);

private:
    C2DPolygon  m_convex_hull_polygon;
    C2DPointSet m_point_cloud;
    Canvas2D    m_canvas, m_canvas_algo_steps;
};

#endif
