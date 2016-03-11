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
    float m_camera_dist;
    float m_scene_width, m_scene_height;
    int m_screen_width, m_screen_height;
    int m_mouselastX, m_mouselastY;
    float4x4 m_proj_mat;
    float m_fov;
    Frustum m_frustum;

private:
    float4x4 modelViewMatrix();

protected:
    bool m_perspective_proj;
    Vec3d m_world_rot_def;
    Vec3d m_world_rot;
    Colour m_bg_col;
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
    virtual const char* getName() const;
    
    //! OpenGL Callbacks

    void GL_Render();
    void GL_Init();
    void GL_Resize(int width, int height);

    //! Events

    virtual bool idle(){return false;}
    virtual void keyEvent(unsigned char key, bool up, int modif);
    virtual void arrowEvent(ArrowDir dir, int modif);
    virtual void mousePressed(int x, int y, int modif);
    virtual void mouseReleased(int x, int y, int modif);
    virtual void mouseMoved(int x, int y, int modif);
    virtual void mouseWheel(int dir, int modif);
    virtual void sliderChanged(int slider_id, float val);
    
    //! Getters

    const Vec3d& getRot() const { return m_world_rot;}
    int getViewportWidth() { return m_screen_width;} // In pixels
    int getViewportHeight() { return m_screen_height;} // In pixels
    float getSceneWidth() { return m_scene_width;} 
    float getSceneHeight() { return m_scene_height;}
    bool fullScreen() { return m_fullscreen; }
    bool createMenus() { return m_create_menus; }
    bool hideLog() { return m_hide_log; }
    bool hideSliders() { return m_hide_sliders; }

    //! Setters

    void setRot(const Vec3d& rot) { m_world_rot = rot;}
    void setCol(const Colour& col) { m_bg_col = col;}
    void setSliderVal(int slider_id, float val);

    //! Helpers

public: 
    void mouse2pix(int &x, int &y);
    Ray unproject(float x, float y);
    bool ctrlDown (int modif) { return modif & (1<<0);}
    bool shiftDown(int modif) { return modif & (1<<1);}
    bool altDown  (int modif) { return modif & (1<<2);}
};

int VVRScene_API mainLoop(int argc, char* argv[], Scene *scene);

}

#endif // VVR_SCENE_H
