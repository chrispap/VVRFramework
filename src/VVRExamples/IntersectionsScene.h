#ifndef SCENE_INTERSECTIONS_H
#define SCENE_INTERSECTIONS_H

#include <scene.h>
#include <canvas.h>
#include <GeoLib.h>

#define APP_TITLE "VVR GeoLab - Intersections"

using namespace std;
using namespace vvr;

class IntersectionsScene : public vvr::Scene
{
public:

    IntersectionsScene();
    const char* getName() const override { return APP_TITLE; }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;

private:
    void SetupTask1();
    void SetupTask2();
    void SetupTask3();
    void HandleTask1(const C2DPoint &p);
    void HandleTask2(const C2DPoint &p);
    void HandleTask3(const C2DPoint &p);
    void HandlePoint(const C2DPoint &p);

private: 
    Canvas2D    canvas0, canvas1, canvas2, canvas3;
    C2DLine     boundary1, boundary2;
    C2DTriangle triangle;
    C2DPoint    A, B, C;
    C2DPointSet points;
    double      path_width;
};

#endif
