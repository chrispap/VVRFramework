#ifndef SCENE_CONVEX_HULL_H
#define SCENE_CONVEX_HULL_H

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/utils.h>
#include <GeoLib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

class ConvexHullScene : public vvr::Scene
{
public:

    ConvexHullScene();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif);

private:
    void ConvexHull_Slow();
    void ConvexHull_Fast();

private:
    C2DPolygon m_convex_hull_polygon;
    C2DPointSet m_point_cloud;
    vvr::Canvas2D m_canvas, m_canvas_algo_steps;
    vvr::Point2D* m_mouse_pos;
};

#endif // SCENE_CONVEX_HULL_H
