#include "eyetracking.h"
#include <utils.h>
#include <canvas.h>
#include <iostream>
#include <fstream>
#include <MathGeoLib.h>
#include <simpleLogger.h>

/////////////////////////////////////////////////////////////////////////////////////////
//! Definitions
/////////////////////////////////////////////////////////////////////////////////////////

#define RAD_SMALL           15
#define RAD_LARGE           22
#define PIXEL_INTERVAL      400
#define TIME_INTERVAL       1500
#define AUTO_TARGET_MOVE    0

/////////////////////////////////////////////////////////////////////////////////////////
//! Static data
/////////////////////////////////////////////////////////////////////////////////////////

vvr::Colour EyeTrackingScene::COL_BG = vvr::Colour::grey;
vvr::Colour EyeTrackingScene::COL_TARGET = vvr::Colour::darkRed;
vvr::Colour EyeTrackingScene::COL_TARGET_ACTIVE = vvr::Colour::magenta;
vvr::Colour EyeTrackingScene::COL_GAZE_FILL = vvr::Colour::yellowGreen;
vvr::Colour EyeTrackingScene::COL_GAZE_LINE = vvr::Colour::black;

/////////////////////////////////////////////////////////////////////////////////////////
//! Setup
/////////////////////////////////////////////////////////////////////////////////////////

EyeTrackingScene::EyeTrackingScene()
{
    startEyeTracker();

    // Init members
    m_bg_col = COL_BG;
    m_fullscreen = true;
    m_create_menus = false;
    m_pause = !AUTO_TARGET_MOVE;
    m_active_target_index = 0;

    // Create targets
    const vvr::Colour col(COL_TARGET);
    const int W = 0.85 / 2 * m_W;
    const int H = 0.85 / 2 * m_H;

    vvr::Circle2D * c;
    
    // Create grid
    for (int h = H; h >= -H; h -= PIXEL_INTERVAL) {
        for (int w = -W; w <= W; w += PIXEL_INTERVAL) {
            c = new vvr::Circle2D(w, h, RAD_LARGE, col);
            m_canvas_bg.add(c);
            m_circles.push_back(c);
        }
    }

    setActiveTarget(0);
}

EyeTrackingScene::~EyeTrackingScene()
{
    stopEyeTracker();
}

/////////////////////////////////////////////////////////////////////////////////////////
//! Eye Tracking
/////////////////////////////////////////////////////////////////////////////////////////

void EyeTrackingScene::startEyeTracker()
{
    // Init EyeTribe
    if (m_api.connect(true)) 
    {
        m_api.add_listener(*this);
        gtl::Screen screen;
        m_api.get_screen(screen);
        m_W = screen.screenresw;
        m_H = screen.screenresh;

        vvr::logi("msec, pixel_x, pixel_y");
    }

}

void EyeTrackingScene::stopEyeTracker()
{
    m_api.remove_listener(*this);
    m_api.disconnect();
}

void EyeTrackingScene::on_gaze_data(gtl::GazeData const &gaze_data)
{
    if (gaze_data.state & gtl::GazeData::GD_STATE_TRACKING_GAZE)
    {
        gtl::Point2D const & coords = gaze_data.avg;
        gtl::Screen screen;
        m_api.get_screen(screen);
        const float w = screen.screenresw;
        const float h = screen.screenresh;
        const float x = coords.x - w / 2;
        const float y = -(coords.y - h / 2);

		echo(x);
		echo(y);
		echo(w);
		echo(h);

        m_gaze_circle = vvr::Circle2D(x, y, RAD_SMALL/2, COL_GAZE_FILL);
        //m_canvas.add(new vvr::Circle2D(m_gaze_circle));
       
        //// Log new gaze data
        //vvr::logi(std::string(" ") + 
        //    std::to_string(gaze_data.time) + "," + 
        //    std::to_string(x) + "," + 
        //    std::to_string(y));

    }

}

void EyeTrackingScene::setActiveTarget(int index)
{
    m_circles.at(m_active_target_index)->setSolidRender(false);
    m_circles.at(m_active_target_index)->setColour(COL_TARGET);
    m_circles.at(m_active_target_index)->r = RAD_LARGE;

    m_active_target_index = index;

    m_circles.at(m_active_target_index)->setSolidRender(true);
    m_circles.at(m_active_target_index)->setColour(COL_TARGET_ACTIVE);
    m_circles.at(m_active_target_index)->r = RAD_LARGE * 1;
}

void EyeTrackingScene::randomTargetJump()
{
    // int index = rand() % m_circles.size();
    int index = (m_active_target_index+1) % m_circles.size();
    setActiveTarget(index);
}

/////////////////////////////////////////////////////////////////////////////////////////
//! Rendering / Animation
/////////////////////////////////////////////////////////////////////////////////////////

void EyeTrackingScene::draw()
{
    enterPixelMode();
    
    m_canvas_bg.draw();
    m_canvas.draw();
    
    vvr::Colour oldcol(m_gaze_circle.colour);
    m_gaze_circle.setSolidRender(true);
    m_gaze_circle.draw();
    m_gaze_circle.setSolidRender(false);
    m_gaze_circle.setColour(COL_GAZE_LINE);
    m_gaze_circle.draw();
    m_gaze_circle.setColour(oldcol);
   
    m_target_circle.draw();
    m_mouse_circle.draw();
    
    returnFromPixelMode();
}

bool EyeTrackingScene::idle()
{
    if (m_pause) return true;

    const float t = vvr::getSeconds();

    // Animate circles
    unsigned long msec = t * 1000;
    static unsigned long last_msec = msec;

    if (msec - last_msec > TIME_INTERVAL) {
        last_msec = msec;
        randomTargetJump();
    }

    return true;

    // Animate a sine
    const float T = 20;
    const float W = getViewportWidth();
    const float H = getViewportHeight();
    const float a = cos(2.0 * math::pi * t / T);
    const int y = H * a * 0.2f;
    const int x = (((int)(W * t / T)) % (int) W) - W / 2;

    m_target_circle = vvr::Circle2D(x, y, RAD_SMALL, vvr::Colour::black);
    m_target_circle.setSolidRender(true);
    m_canvas.add(new vvr::Circle2D(m_target_circle));
    m_target_circle.setColour(vvr::Colour::red);
 
    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//! UI Handling
/////////////////////////////////////////////////////////////////////////////////////////

void EyeTrackingScene::mousePressed(int x, int y, int modif)
{
    int i=0;
    //for (auto c : m_circles) {
    //    if (C2DCircle(C2DPoint(c->x, c->y), c->r).Contains(C2DPoint(x,y))) {
    //        setActiveTarget(i);
    //        return;
    //    }
    //    ++i;
    //}

    return;

    m_mouse_circle = vvr::Circle2D(x, y, RAD_SMALL, vvr::Colour::black);
    m_mouse_circle.setSolidRender(true);
    m_canvas.add(new vvr::Circle2D(m_mouse_circle));
    m_mouse_circle.setColour(vvr::Colour::blue);
}

void EyeTrackingScene::mouseMoved(int x, int y, int modif)
{
    mousePressed(x, y, modif);
}

void EyeTrackingScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);

    key = ::tolower(key);

    switch (key)
    {
    case ' ':
        m_pause = !m_pause;
        break;
    default:
        break;
    }

}

void EyeTrackingScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::LEFT) 
    {
        //...
    }
    else if (dir == vvr::RIGHT) 
    {
        randomTargetJump();
    }
}

void EyeTrackingScene::reset()
{
    vvr::Scene::reset();
    m_canvas.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////
//! Main
/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    vvr::Scene *s = new EyeTrackingScene();
    return vvr::mainLoop(argc, argv, s);
}
