#ifndef SCENE_01_H
#define SCENE_01_H

#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include "canvas.h"

#define APP_TITLE "Simple 2D Drawing Example"

using namespace std;
using namespace vvr;

class Scene01 : public vvr::Scene
{
public: 
    Scene01();
    const char* getName() const override { return APP_TITLE;}

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

#endif
