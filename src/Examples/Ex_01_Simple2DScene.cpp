#include "Ex_01_Simple2DScene.h"
#include "utils.h"
#include "canvas.h"

#include <MathGeoLib/MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using namespace vvr;

#define CONFIGFILEPATH "../../config/settings.txt"

/**
 * Create scene.
 * Called automatically ONCE, in the beginning of the program.
 */
Simple2DScene::Simple2DScene()
{
    m_rad = 20;
    m_bg_col = Colour::black;
    m_pts.resize(1);
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
    m_canvas.draw();

    // Draw all line-strips (Contours)
    for (int ci=0; ci<m_pts.size(); ci++) {
        for (int pi=0; pi<m_pts[ci].size(); pi++) {
            Vec3d &p1 = m_pts[ci][pi];
            Vec3d &p2 = m_pts[ci][(pi+1)%m_pts[ci].size()];
            Colour line_col = Colour::yellow;
            if (ci==m_pts.size()-1 && pi==m_pts[ci].size()-1)
                line_col = Colour::red;
            LineSeg2D(p1.x, p1.y, p2.x, p2.y, line_col).draw();
            Point2D(p1.x, p1.y, Colour::yellow).draw();

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

    if (modif) {
        m_pts.resize(m_pts.size()+1);
    }

    m_pts.back().push_back(Vec3d(x,y,0));
}

void Simple2DScene::mouseMoved(int x, int y, int modif)
{
    mouse2pix(x,y);
}

/**
 * Overload reset of base class.
 * Clear canvas.
 */
void Simple2DScene::reset()
{
    Scene::reset();
    m_canvas.clear();
    m_pts.clear();
    m_pts.resize(1);
}

void Simple2DScene::keyEvent(unsigned char key, bool up, int x, int y, int modif)
{
    Scene::keyEvent(key, up, x, y, modif);

    switch (key) {
    case 's': saveContoursToFile(); break;
    case 'd': m_pts.back().resize(std::max(1, (int) m_pts.back().size()-1));
    }

}

void Simple2DScene::saveContoursToFile()
{
    string filename = "contours.dat";
    filename = vvr::getBasePath() + "resources/data/" + filename;
    std::cout << "Saving to " << filename << std::endl;

    FILE* file = fopen(filename.c_str(), "w");
    if (!file) throw "Cannot open <" + filename + "> for writing";

    for (int ci=0; ci<m_pts.size(); ci++) {
        fprintf(file, "---CONTOUR-LINE---\n");
        for (int pi=0; pi<m_pts[ci].size(); pi++) {
            Vec3d &p = m_pts[ci][pi];
            fprintf(file, "%f %f\n", p.x, p.y);
        }
    }

    fclose(file);
}
