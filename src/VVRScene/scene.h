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

class VVRScene_API Scene
{
protected:
    Vec3d   m_globRotDef;
    Vec3d   m_globRot;
    float   m_scene_width;
    float   m_scene_height;
    float   m_camera_dist;
    int     m_screen_width;
    int     m_screen_height;
    bool    m_perspective_proj;
    Colour  m_bg_col;

protected:
    virtual void draw() = 0;
    virtual void reset();
    void drawAxes();
    void enterPixelMode();
    void returnFromPixelMode();

private:
    int m_mouselastX, m_mouselastY;

public:
    Scene();
    virtual ~Scene(){}
    void GL_Render();
    void GL_Init();
    void GL_Resize(int width, int height);
    virtual bool idle(){return false;}
    virtual void keyEvent(unsigned char key, bool up, int modif);
    virtual void arrowEvent(ArrowDir dir, int modif);
    virtual void mousePressed(int x, int y, int modif);
    virtual void mouseMoved(int x, int y, int modif);
    virtual void mouseWheel(int dir, int modif);
    virtual const char* getName() const;
    const Vec3d& getRot() const { return m_globRot; }
    void setRot(const Vec3d& rot) { m_globRot = rot; }
    void setCol(const Colour& col) { m_bg_col = col; }
    void mouse2pix(int &x, int &y);
};

/* This will be the entry point of client applications. */
int VVRScene_API mainLoop(int argc, char* argv[], Scene *scene);

}

#endif
