#ifndef SCENE_INTERSECTIONS_H
#define SCENE_INTERSECTIONS_H

#include <vvr/scene.h>
#include <vvr/drawing.h>
#include <vvr/utils.h>
#include <GeoLib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <algorithm>

class SceneCollinear : public vvr::Scene
{
public:

    SceneCollinear();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

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
    vvr::Canvas2D canvas0, canvas1;
    C2DLine boundary1, boundary2;
    C2DPointSet points;
};

#endif
