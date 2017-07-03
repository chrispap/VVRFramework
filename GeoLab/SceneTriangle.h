#ifndef SCENE_TRIANGLE_H
#define SCENE_TRIANGLE_H

#include <vvr/scene.h>
#include <vvr/drawing.h>

class TriangleScene : public vvr::Scene
{
public:
    TriangleScene();
    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;

private:
    std::vector<vvr::Point2D*> m_pts;
    vvr::Canvas2D m_canvas, m_canvas_pts;
    vvr::Point2D *m_point_grabbed;
    void createShapesFromPoints();
};

#endif // SCENE_TRIANGLE_H
