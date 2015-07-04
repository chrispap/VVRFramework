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
#define PIXEL_INTERVAL      300
#define TIME_INTERVAL       1888
#define AUTO_TARGET_MOVE    1

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
    // Init members
    m_bg_col = COL_BG;
    m_fullscreen = true;
    m_create_menus = false;
    m_pause = !AUTO_TARGET_MOVE;
    m_active_target_index = 0;

	// Init Eye Tracker
    startEyeTracker();
}

EyeTrackingScene::~EyeTrackingScene()
{
    stopEyeTracker();
}

void EyeTrackingScene::resize()
{
	// Get size
	m_W = getViewportWidth();
	m_H = getViewportHeight();

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

/////////////////////////////////////////////////////////////////////////////////////////
//! Eye Tracking
/////////////////////////////////////////////////////////////////////////////////////////

void EyeTrackingScene::startEyeTracker()
{
    if (m_api.connect(true)) 
    {
        m_api.add_listener(*this);
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
		const float w = getViewportWidth();
		const float h = getViewportHeight();
        gtl::Point2D const &coords = gaze_data.avg;
		const float x = coords.x - w / 2;
        const float y = -(coords.y - h / 2);

        m_gaze_circle = vvr::Circle2D(x, y, RAD_SMALL/2, COL_GAZE_FILL);
        //m_canvas.add(new vvr::Circle2D(m_gaze_circle));
       
		// Log gaze data
		//vvr::logi(std::string(" ") + SSTR(gaze_data.time) + "," + SSTR(x) + "," + SSTR(y));
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
	for (auto ci = m_circles.begin(); ci!=m_circles.end(); ++ci) {
		auto c = *ci;
        if (C2DCircle(C2DPoint(c->x, c->y), c->r).Contains(C2DPoint(x,y))) {
            setActiveTarget(i);
            return;
        }
        ++i;
    }

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
