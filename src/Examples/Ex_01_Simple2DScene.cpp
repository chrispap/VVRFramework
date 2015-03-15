#include "Ex_01_Simple2DScene.h"
#include "utils.h"
#include "canvas.h"

#include <MathGeoLib/MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <string>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using namespace vvr;

#define APP_TITLE "Simple 2D Drawing"
#define CONFIGFILEPATH "../../config/settings.txt"

const char* Simple2DScene::getName() const
{
    return APP_TITLE;
}

/**
 * Create scene.
 * Called automatically ONCE, in the beginning of the program.
 */
Simple2DScene::Simple2DScene()
{
    m_rad = 20;
    m_bg_col = Colour::grey;
    m_curve_count = 0;

    // Add 5 circles to our canvas.
    m_canvas.add(new Circle2D( -40, -20, 40, Colour::red));
    m_canvas.add(new Circle2D( -20,  20, 40, Colour::green));
    m_canvas.add(new Circle2D(   0, -20, 40, Colour::blue));
    m_canvas.add(new Circle2D(  20,  20, 40, Colour::black));
    m_canvas.add(new Circle2D(  40, -20, 40, Colour::yellow));

    // Add some points
    for (int i=0; i<10; i++) {
        m_canvas.add(new Point2D(i*10,i*10, Colour::white));
    }

    // Create new frame so that everything drawn from now on, go to a different frame
    m_canvas.newFrame(true);
}

/**
 * Draw scene.
 * Called automatically every time the scene may need update.
 * Such times are:
 *  - The firt render of the scene in the beginning.
 *  - After every user event (click, resize, etc.)
 */
void Simple2DScene::draw()
{
    enterPixelMode();
    // m_canvas.draw();
    for (int i=0; i<m_pts.size(); i++) {
        for (int j=0; j<m_pts[i].size(); j++) {
            Vec3d &p1 = m_pts[i][j];
            Vec3d &p2 = m_pts[i][(j+1)%m_pts[i].size()];
            LineSeg2D line(p1.x, p1.y, p2.x, p2.y, Colour::blue);
            line.draw();
        }
    }

    returnFromPixelMode();
}

/**
 * Transform mouse click coords to pixel scene coords.
 * This is needed because we have moved the origin (0,0) at the viewport center.
 * @param x
 * @param y
 */
void Simple2DScene::mouse2pix(int &x, int &y)
{
    x -= m_screen_width/2;
    y -= m_screen_height/2;
}

/**
 * @brief Simple2DScene::mousePressed
 * @param x
 * @param y
 * @param Modifier flag. Each bit represents something.
 */
void Simple2DScene::mousePressed(int x, int y, int modif)
{
    mouse2pix(x,y);

    m_pts.resize(++m_curve_count);
    m_pts[m_curve_count-1].push_back(Vec3d(x,y,0));

    m_canvas.newFrame();
    m_canvas.add(new Point2D(x,y, Colour::white));
}

void Simple2DScene::mouseMoved(int x, int y, int modif)
{
    mouse2pix(x,y);

    m_pts[m_curve_count-1].push_back(Vec3d(x,y,0));

    m_canvas.add(new Point2D(x,y, Colour::white));    }

void Simple2DScene::mouseWheel(int dir, int modif)
{
    if (dir>0) {
        m_rad += 1;
    }
    else {
        m_rad -= 1;
        if (m_rad<=0) m_rad = 1;
    }

}

void Simple2DScene::arrowEvent(ArrowDir dir, int modif)
{
    if (dir==LEFT) {
        m_canvas.prev();
    }
    else if (dir==RIGHT) {
        m_canvas.next();
    }

}

/**
 * Overload reset of base class.
 * Clear canvas.
 */
void Simple2DScene::reset()
{
    Scene::reset();
    m_canvas.clear();
}
