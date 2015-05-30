#ifndef SCENE_INTERSECTIONS_H
#define SCENE_INTERSECTIONS_H

#include <scene.h>
#include <canvas.h>
#include <GeoLib.h>

#define APP_TITLE "VVR GeoLab - Intersections"

using namespace std;
using namespace vvr;

class CollinearScene : public vvr::Scene
{
public:

    CollinearScene();
    const char* getName() const override { return APP_TITLE; }

protected:
    void draw() override;
    void reset() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;

private:
    void SetupTask();
    void HandleTask(const C2DPoint &p);
    void HandlePoint(const C2DPoint &p);

private: 
    Canvas2D    canvas0, canvas1;
    C2DLine     boundary1, boundary2;
    C2DPointSet points;
};

#endif
