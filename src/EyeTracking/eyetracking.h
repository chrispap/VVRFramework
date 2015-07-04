#ifndef VVR_EYE_TRACKING_H
#define VVR_EYE_TRACKING_H

//#define BOOST_ALL_NO_LIB

#include "settings.h"
#include <scene.h>
#include <mesh.h>
#include <canvas.h>
#include <vector>
#include <gazeapi.h>

class EyeTrackingScene : public vvr::Scene, public gtl::IGazeListener
{
public:
    EyeTrackingScene();
    ~EyeTrackingScene();
    const char* getName() const override { return "Eye Tracking Scene"; }

protected:
    void draw() override;
    void reset() override;
	void resize() override;
    bool idle() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;

private:
    void startEyeTracker();
    void stopEyeTracker();
    void on_gaze_data(gtl::GazeData const & gaze_data);
    void setActiveTarget(int index);
    void randomTargetJump();

private:
    std::vector<vvr::Circle2D*> m_circles;
    int m_active_target_index;
    bool m_pause;
    int m_W, m_H;
    gtl::GazeApi m_api;
    vvr::Canvas2D m_canvas, m_canvas_bg;
    vvr::Circle2D m_target_circle, m_gaze_circle, m_mouse_circle;

private: 
    static vvr::Colour COL_BG;
    static vvr::Colour COL_TARGET;
    static vvr::Colour COL_TARGET_ACTIVE;
    static vvr::Colour COL_GAZE_FILL, COL_GAZE_LINE;
};

#endif
