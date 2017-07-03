#ifndef SCENE_AREA_H
#define SCENE_AREA_H

#include <vvr/scene.h>

class SceneArea : public vvr::Scene
{
public:
    SceneArea();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected:
    void draw() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;

private:
    float m_rad;
    int m_x;
    int m_y;
};

#endif // SCENE_AREA_H
