#ifndef VVR_SCENE_H
#define VVR_SCENE_H

#include "vvrscenedll.h"
#include "geom.h"
#include <MathGeoLib.h>

namespace vvr {

enum ArrowDir {
    UP = 0,
    DOWN,
    RIGHT,
    LEFT
};

class VVRScene_API Scene
{
private:
    Frustum m_frustum;
    float m_fov;
    float m_camera_dist;
    float m_scene_width, m_scene_height;
    int m_screen_width, m_screen_height;
    int m_mouselastX, m_mouselastY;

protected:
    Colour m_bg_col;
    bool m_perspective_proj;
    bool m_fullscreen;
    bool m_create_menus;
    bool m_hide_log;
    bool m_hide_sliders;

protected:
    virtual void draw() = 0;
    virtual void reset();
    virtual void resize(){}

protected:
    void drawAxes();
    void enterPixelMode();
    void returnFromPixelMode();

public:
    Scene();
    virtual ~Scene(){}
    
    virtual const char* getName() const {
        return "VVR Framework Scene";
    }

    //! OpenGL Callbacks

    void GL_Render();
    void GL_Init();
    void GL_Resize(int width, int height);

    //! Events

    virtual bool idle(){ return false; }
    virtual void keyEvent(unsigned char key, bool up, int modif);
    virtual void arrowEvent(ArrowDir dir, int modif);
    virtual void mousePressed(int x, int y, int modif);
    virtual void mouseReleased(int x, int y, int modif);
    virtual void mouseMoved(int x, int y, int modif);
    virtual void mouseWheel(int dir, int modif);
    virtual void sliderChanged(int slider_id, float val);

    //! Getters

    int getViewportWidth() { return m_screen_width; } // In pixels
    int getViewportHeight() { return m_screen_height; } // In pixels
    float getSceneWidth() { return m_scene_width; }
    float getSceneHeight() { return m_scene_height; }
    Frustum getFrustum() { return m_frustum; };
    bool fullScreen() { return m_fullscreen; }
    bool createMenus() { return m_create_menus; }
    bool hideLog() { return m_hide_log; }
    bool hideSliders() { return m_hide_sliders; }

    //! Setters
    
    void setFrustum(const Frustum &frustum) { m_frustum = frustum; }
    void setCol(const Colour& col) { m_bg_col = col; }
    void setSliderVal(int slider_id, float val);

public:
    
    void setCameraPos(const vec &pos);

    //! Helpers

public:
    void mouse2pix(int &x, int &y);
    Ray unproject(int x, int y);
    bool ctrlDown(int modif) { return modif & (1 << 0); }
    bool shiftDown(int modif) { return modif & (1 << 1); }
    bool altDown(int modif) { return modif & (1 << 2); }
};

int VVRScene_API mainLoop(int argc, char* argv[], Scene *scene);

}

#endif // VVR_SCENE_H
