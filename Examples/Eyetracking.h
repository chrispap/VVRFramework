#ifndef VVR_EYE_TRACKING_H
#define VVR_EYE_TRACKING_H

#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/canvas.h>
#include <vector>
//#include <gazeapi.h>
#include <opencv2/opencv.hpp>

class EyeTrackingScene : public vvr::Scene //, public gtl::IGazeListener
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
    void mouseWheel(int dir, int modif);
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void sliderChanged(int slider_id, float val) override;

private:
    void createScene();
    void storeFrame();
    void setActiveTarget(int index);
    void targetJump(bool random=false);

private:
    std::vector<vvr::Circle2D*> m_circles;
    int m_active_target_index;
    bool m_pause;
    int m_W, m_H;
    int m_GridCount_Hor, m_GridCount_Ver;
    int m_frame_id;
    cv::VideoCapture m_cap;
    vvr::Canvas2D m_canvas, m_canvas_bg;
    vvr::Circle2D m_target_circle, m_gaze_circle, m_mouse_circle;

private:
    static vvr::Colour COL_BG;
    static vvr::Colour COL_TARGET;
    static vvr::Colour COL_TARGET_ACTIVE;
    static vvr::Colour COL_GAZE_FILL, COL_GAZE_LINE;

    void startEyeTracker();
    void stopEyeTracker();

#ifdef USE_GAZE_API
    gtl::GazeApi m_api;
    void on_gaze_data(gtl::GazeData const & gaze_data);
#endif
};

#endif
