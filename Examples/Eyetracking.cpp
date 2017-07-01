#include "Eyetracking.h"
#include <vvr/utils.h>
#include <vvr/canvas.h>
#include <vvr/logger.h>
#include <iostream>
#include <fstream>
#include <MathGeoLib.h>

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////
//! Definitions
/////////////////////////////////////////////////////////////////////////////////////////

static int RAD_GAZE_MARKER = 15;
static int RAD_GRID_CIRCLE = 8;
static int PIXEL_INTERVAL = 30;
static int TIME_INTERVAL = 1000;
static int RANDOM_JUMPS = 1;
static int ANIMATE_SINE = 0;
static int AUTOSTART = 0;

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

EyeTrackingScene::EyeTrackingScene() :
    m_cap(0),
    m_frame_id(0),
    m_pause(!AUTOSTART),
    m_active_target_index(0)
{
    m_bg_col = COL_BG;
    m_fullscreen = false;
    m_create_menus = false;
    m_hide_log = false;
    m_hide_sliders = false;

    // Start Eye Tracking
    startEyeTracker();

    // Ensure output path exists
    std::string filepath = vvr::getBasePath() + "capture/";
    vvr::mkdir(filepath);

    // Capture a frame so that the camera will calibrate brightness etc.
    cv::Mat frame;
    m_cap >> frame;
}

EyeTrackingScene::~EyeTrackingScene()
{
    stopEyeTracker();
}

void EyeTrackingScene::createScene()
{
    m_canvas_bg.clear();
    m_circles.clear();

    // Create targets
    const vvr::Colour col(COL_TARGET);
    const int W = 0.85 / 2 * m_W;
    const int H = 0.85 / 2 * m_H;

    // Create grid
    m_GridCount_Ver = m_GridCount_Hor = 0;
    for (int h = H; h >= -H; h -= PIXEL_INTERVAL) {
        for (int w = -W; w <= W; w += PIXEL_INTERVAL) {
            vvr::Circle2D * c = new vvr::Circle2D(w, h, RAD_GRID_CIRCLE, col);
            m_canvas_bg.add(c);
            m_circles.push_back(c);
            ++m_GridCount_Hor;
        }
        ++m_GridCount_Ver;
    }

    // Because `m_GridCount_Hor` was incremented too many times...
    m_GridCount_Hor /= m_GridCount_Ver;
}

void EyeTrackingScene::resize()
{
    m_W = getViewportWidth();
    m_H = getViewportHeight();
    createScene();
    setActiveTarget(0);
    std::cout << ("GridCount_Hor: " + std::to_string(m_GridCount_Hor));
    std::cout << ("GridCount_Ver: " + std::to_string(m_GridCount_Ver));
}

/////////////////////////////////////////////////////////////////////////////////////////
//! Eye Tracking
/////////////////////////////////////////////////////////////////////////////////////////


void EyeTrackingScene::startEyeTracker()
{
#ifdef USE_GAZE_API
    if (m_api.connect(true))
    {
        m_api.add_listener(*this);
    }
#endif
}

void EyeTrackingScene::stopEyeTracker()
{
#ifdef USE_GAZE_API
    m_api.remove_listener(*this);
    m_api.disconnect();
#endif
}

#ifdef USE_GAZE_API
void EyeTrackingScene::on_gaze_data(gtl::GazeData const &gaze_data)
{
    if (gaze_data.state & gtl::GazeData::GD_STATE_TRACKING_GAZE)
    {
        const float w = getViewportWidth();
        const float h = getViewportHeight();
        gtl::Point2D const &coords = gaze_data.avg;
        const float x = coords.x - w / 2;
        const float y = -(coords.y - h / 2);

        m_gaze_circle = vvr::Circle2D(x, y, RAD_GAZE_MARKER/2, COL_GAZE_FILL);
        m_canvas.add(new vvr::Circle2D(m_gaze_circle));

        // Log gaze data
        //std::cout << (std::string(" ") + SSTR(gaze_data.time) + "," + SSTR(x) + "," + SSTR(y));
    }
}
#endif

void EyeTrackingScene::storeFrame()
{
    // Save frame from camera for current target_index
    //cout << ("FRAME: " + std::to_string(m_frame_id) + "-TARGET: " + std::to_string(m_active_target_index));
    std::string filepath = vvr::getBasePath() + "capture/" + "frame_" + std::to_string(m_frame_id) + ".png";
    cv::Mat frame;
    float sec = vvr::getSeconds();
    m_cap >> frame;
    sec = vvr::getSeconds() - sec;
    //cout << "msec: " << (int) (sec * 1000) << "ms" << endl;
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    cv::imwrite(filepath, frame, compression_params);
}

void EyeTrackingScene::setActiveTarget(int index)
{
    m_circles.at(m_active_target_index)->setSolidRender(false);
    m_circles.at(m_active_target_index)->setColour(COL_TARGET);
    m_circles.at(m_active_target_index)->r = RAD_GRID_CIRCLE;
    m_active_target_index = index;
    m_circles.at(m_active_target_index)->setSolidRender(true);
    m_circles.at(m_active_target_index)->setColour(COL_TARGET_ACTIVE);
    m_circles.at(m_active_target_index)->r = RAD_GRID_CIRCLE * 1;
}

void EyeTrackingScene::targetJump(bool random)
{
    storeFrame();

    int target_index = m_active_target_index;

    // Jump to next circle
    if (random) {
        while ((target_index = rand() % m_circles.size()) == m_active_target_index);
    }
    else {
        target_index = (m_active_target_index + 1) % m_circles.size();
    }

    setActiveTarget(target_index);

    ++m_frame_id;
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

    // Update time
    const float t = vvr::getSeconds();
    unsigned long msec = t * 1000;
    static unsigned long last_msec = msec;

    // Animate
    if (msec - last_msec > TIME_INTERVAL) {
        last_msec = msec;
        targetJump(RANDOM_JUMPS);
    }

    // Animate a sine
    if (ANIMATE_SINE) {
        const float T = 20;
        const float W = getViewportWidth();
        const float H = getViewportHeight();
        const float a = cos(2.0 * math::pi * t / T);
        const int y = H * a * 0.2f;
        const int x = (((int)(W * t / T)) % (int)W) - W / 2;
        m_target_circle = vvr::Circle2D(x, y, RAD_GRID_CIRCLE, vvr::Colour::black);
        m_target_circle.setSolidRender(true);
        m_canvas.add(new vvr::Circle2D(m_target_circle));
        m_target_circle.setColour(vvr::Colour::red);
    }

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
}

void EyeTrackingScene::mouseMoved(int x, int y, int modif)
{
    mousePressed(x, y, modif);
}

void EyeTrackingScene::mouseWheel(int dir, int modif)
{
    RAD_GRID_CIRCLE += dir;
    createScene();
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
        targetJump(RANDOM_JUMPS);
    }
}

void EyeTrackingScene::sliderChanged(int slider_id, float val)
{
    static auto RAD_GRID_CIRCLE_DEFAULT = RAD_GRID_CIRCLE;
    static auto PIXEL_INTERVAL_DEFAULT = PIXEL_INTERVAL;
    static auto TIME_INTERVAL_DEFAULT = TIME_INTERVAL;

    switch (slider_id)
    {
    case 0:  RAD_GRID_CIRCLE = RAD_GRID_CIRCLE_DEFAULT *val * 25; break;
    case 1:  PIXEL_INTERVAL = PIXEL_INTERVAL_DEFAULT * val * 10; break;
    case 2:  TIME_INTERVAL = TIME_INTERVAL_DEFAULT * val * 4; echo(TIME_INTERVAL);  break;
    default: return;
    }

    if (PIXEL_INTERVAL < 5) {
        PIXEL_INTERVAL = PIXEL_INTERVAL_DEFAULT;
    }

    createScene();
}

void EyeTrackingScene::reset()
{
    m_active_target_index = 0;
    vvr::Scene::reset();
    m_canvas.clear();
}

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new EyeTrackingScene());
}
