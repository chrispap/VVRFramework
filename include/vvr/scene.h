#ifndef VVR_SCENE_H
#define VVR_SCENE_H

#include "vvrframework_DLL.h"
#include <vvr/drawing.h>
#include <vvr/command.h>
#include <MathGeoLib.h>

namespace vvr
{
    enum ArrowDir { UP = 0, DOWN, RIGHT, LEFT };

    class VVRFramework_API Scene
    {
    public:
        Scene();
        virtual ~Scene() {}
        virtual const char* getName() const {
            return "VVRFramework Application";
        }

        static inline bool ctrlDown(int modif) { return modif & (1 << 0); }
        static inline bool shiftDown(int modif) { return modif & (1 << 1); }
        static inline bool altDown(int modif) { return modif & (1 << 2); }

    protected:
        /*---[Events]-------------------------------------------------------------------*/
        virtual bool idle() { return false; }
        virtual void keyEvent(unsigned char key, bool up, int modif);
        virtual void arrowEvent(ArrowDir dir, int modif);
        virtual void mousePressed(int x, int y, int modif);
        virtual void mouseReleased(int x, int y, int modif);
        virtual void mouseMoved(int x, int y, int modif);
        virtual void mouseHovered(int x, int y, int modif);
        virtual void mouseWheel(int dir, int modif);
        virtual void sliderChanged(int slider_id, float val);

        /*---[Getters / Setters]--------------------------------------------------------*/
        Axes& getGlobalAxes() { return m_axes; }
        math::Frustum getFrustum() { return m_frustum; }
        int getViewportWidth() const { return m_screen_width; } // In pixels
        int getViewportHeight() const { return m_screen_height; } // In pixels
        float getSceneWidth() const { return m_scene_width; }
        float getSceneHeight() const { return m_scene_height; }
        float getCameraDist() const { return m_camera_dist; }
        bool getFullScreen() const { return m_fullscreen; }
        bool getCreateMenus() const { return m_create_menus; }
        bool shouldShowLog() const { return m_show_log; }
        bool shouldShowSliders() const { return m_show_sliders; }
        void setFrustum(const math::Frustum &frustum) { m_frustum = frustum; }
        void setSliderVal(int slider_id, float val);
        void setCameraPos(const math::vec &pos);

        /*---[Virtual]------------------------------------------------------------------*/
        virtual void draw() = 0;
        virtual void reset();
        virtual void resize();

        /*---[Helpers]------------------------------------------------------------------*/
        math::Ray unproject(int x, int y);
        void mouse2pix(int &x, int &y);
        void pix2mouse(int &x, int &y);
        void enterPixelMode();
        void exitPixelMode();
        void drawAxes();

    private:
        /*---[OpenGL Callbacks]---------------------------------------------------------*/
        void glRender();
        void glInit();
        void glResize(int width, int height);

        /*---[Data]---------------------------------------------------------------------*/
    protected:
        Colour          m_bg_col;
        bool            m_perspective_proj;
        bool            m_fullscreen;
        bool            m_create_menus;
        bool            m_show_log;
        bool            m_show_sliders;
        bool            m_first_resize;

    protected:
        MacroCmd        cursorShow;
        MacroCmd        cursorHide;
        MacroCmd        cursorHand;
        MacroCmd        cursorGrab;

    private:
        math::Frustum   m_frustum;
        math::Frustum   m_frustum_mlstn;
        Axes            m_axes;
        float           m_fov;
        float           m_camera_dist;
        float           m_scene_width;
        float           m_scene_height;
        int             m_screen_width;
        int             m_screen_height;
        int             m_mouse_x;
        int             m_mouse_y;
        char            m_mouse_op;
        float           m_mouse_depth;

        /*---[Friends]------------------------------------------------------------------*/
        friend class GlWidget;
        friend class Window;
    };

    int VVRFramework_API  main_with_scene(int argc, char* argv[], Scene *scene);
    void VVRFramework_API set_status_bar_msg(const std::string &msg);
    void VVRFramework_API clear_status_bar();
    void VVRFramework_API get_mouse_xy(int &x, int &y);
}

#endif
