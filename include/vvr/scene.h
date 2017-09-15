#ifndef VVR_SCENE_H
#define VVR_SCENE_H

#include "vvrframework_DLL.h"
#include <vvr/drawing.h>
#include <MathGeoLib.h>

namespace vvr {

    enum ArrowDir { UP = 0, DOWN, RIGHT, LEFT };

    class vvrframework_API Scene
    {
    public:
        Scene();
        virtual ~Scene() {}
        virtual const char* getName() const {
            return "VVRFramework Application";
        }

        static bool ctrlDown(int modif) { return modif & (1 << 0); }
        static bool shiftDown(int modif) { return modif & (1 << 1); }
        static bool altDown(int modif) { return modif & (1 << 2); }

    protected:
        /*--- [Events] -----------------------------------------------------------------*/
        virtual bool idle() { return false; }
        virtual void keyEvent(unsigned char key, bool up, int modif);
        virtual void arrowEvent(ArrowDir dir, int modif);
        virtual void mousePressed(int x, int y, int modif);
        virtual void mouseReleased(int x, int y, int modif);
        virtual void mouseMoved(int x, int y, int modif);
        virtual void mouseWheel(int dir, int modif);
        virtual void sliderChanged(int slider_id, float val);

        /*--- [Getters / Setters] ------------------------------------------------------*/
        math::Frustum getFrustum() { return m_frustum; }
        int getViewportWidth() { return m_screen_width; } // In pixels
        int getViewportHeight() { return m_screen_height; } // In pixels
        float getSceneWidth() { return m_scene_width; }
        float getSceneHeight() { return m_scene_height; }
        bool getFullScreen() { return m_fullscreen; }
        bool getCreateMenus() { return m_create_menus; }
        bool getHideLog() { return m_hide_log; }
        bool getHideSliders() { return m_hide_sliders; }
        Axes* getGlobalAxes() { return new Axes(2.0 *getSceneWidth()); }
        void setFrustum(const math::Frustum &frustum) { m_frustum = frustum; }
        void setSliderVal(int slider_id, float val);
        void setCameraPos(const math::vec &pos);

        /*--- [Virtual] ----------------------------------------------------------------*/
        virtual void draw() = 0;
        virtual void reset();
        virtual void resize() {}

        /*--- [Helpers] ----------------------------------------------------------------*/
        math::Ray unproject(int x, int y);
        void mouse2pix(int &x, int &y);
        void enterPixelMode();
        void exitPixelMode();
        void drawAxes();

    private:
        /*--- [OpenGL Callbacks] -------------------------------------------------------*/
        void glRender();
        void glInit();
        void glResize(int width, int height);

    protected:
        Colour m_bg_col;
        bool m_perspective_proj;
        bool m_fullscreen;
        bool m_create_menus;
        bool m_hide_log;
        bool m_hide_sliders;
        bool m_first_resize;

    private:
        math::Frustum m_frustum;
        float m_fov;
        float m_camera_dist;
        float m_scene_width;
        float m_scene_height;
        int m_screen_width;
        int m_screen_height;
        int m_mouse_x;
        int m_mouse_y;

        friend class GlWidget;
        friend class Window;
    };

    int vvrframework_API mainLoop(int argc, char* argv[], Scene *scene);
}

#endif
