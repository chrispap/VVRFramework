#ifndef __SCENE_H__
#define __SCENE_H__

#include "vvrscenedll.h"

#include "geom.h"

namespace vvr {

enum ArrowDir {
    UP = 0,
    DOWN,
    RIGHT,
    LEFT
};

class vvrscene_API Scene
{
protected:
    bool perspective_proj;
    double scene_width, scene_height, camera_dist;
    int mouselastX, mouselastY;
    int mouseCurrX, mouseCurrY;
    int screen_width, screen_height;
    Vec3d globRot, globRotDef;
    ColRGB bgCol;

    void enterPixelMode();
    void returnFromPixelMode();
    void drawAxes();

    virtual void reset() { globRot = globRotDef;}
    virtual void draw() = 0;
    virtual void load() = 0;

public:
    Scene();

    void GL_Render();
    virtual void GL_Init();
    virtual void GL_Resize(int width, int height);

    virtual bool idle(){return false;}
    virtual void keyEvent(unsigned char key, bool up, int x, int y, int modif);
    virtual void arrowEvent(ArrowDir dir, int modif);
    virtual void mousePressed(int x, int y, int modif);
    virtual void mouseMoved(int x, int y, int modif);
    virtual void mouseWheel(int dir, int modif);

    const Vec3d& getRot() const {return globRot;}
    void setRot(const Vec3d& rot) {globRot = rot;}
    void setCol(const ColRGB& col) {bgCol = col;}

    virtual const char* getName() const;
};

}

#endif
