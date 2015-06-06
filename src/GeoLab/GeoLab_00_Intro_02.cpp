#include "GeoLab_00_Intro_02.h"
#include "utils.h"
#include "canvas.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace vvr;
using namespace std;

#define SQUARE(x) ((x)*(x))

Scene02::Scene02()
{
    m_bg_col = Colour(0x44, 0x44, 0x44);
    m_pts.resize(1);
}

void Scene02::draw()
{
    enterPixelMode();

    // Draw all line-strips. (Contours)
    Colour line_col;

    for (int ci=0; ci<m_pts.size(); ci++) 
    {
        for (int pi=0; pi<m_pts[ci].size(); pi++) 
        {    
            Point2D &p1 = m_pts[ci][pi];
            Point2D &p2 = m_pts[ci][(pi+1)%m_pts[ci].size()];
            
            LineSeg2D(p1.x, p1.y, p2.x, p2.y, p1.colour).draw();
            p1.draw();
        }
    }

    returnFromPixelMode();
}

void Scene02::mousePressed(int x, int y, int modif)
{
    Scene::mousePressed(x, y, modif);

    if (modif) {
        m_pts.resize(m_pts.size()+1);
    }

    m_pts.back().push_back(Point2D(x,y));
}

void Scene02::mouseMoved(int x, int y, int modif)
{
    Scene::mouseMoved(x, y, modif);

    // An den yparxei kanena simeio, den ginetai elegxo apostasis
    if (m_pts.back().empty()) 
    {
        m_pts.back().push_back(Point2D(x,y));
    }
    else
    {
        // last_x, last_y : Coordinates of last point.
        const double min_dist = 10;
        const double last_x = m_pts.back().back().x;
        const double last_y = m_pts.back().back().y;

        // TASK:
        // Ypologiste tin apostasi twn simeiwn:  (x, y) , (last_x, last_y)
        double dist = sqrt(SQUARE(x-last_x) + SQUARE(y-last_y));
        
        if (dist > min_dist) {
            Colour col = x > last_x ? Colour::red : Colour::blue;
            m_pts.back().push_back(Point2D(x,y, col));
        }

    }

}

void Scene02::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);

    switch (key) 
    {
    case 'd':
        if (m_pts.back().size()>0)
            m_pts.back().resize(m_pts.back().size()-1);
        else
            m_pts.resize(max(1, (int) m_pts.size()-1));
        break;
    }

}

void Scene02::reset()
{
    Scene::reset();

    // Fist clear, then resize. 
    // This way we make sure that m_pts[0] is cleared.
    m_pts.clear(); 
    m_pts.resize(1);
}

/* Application Entry Point */

int main(int argc, char* argv[])
{
    vvr::mainLoop(argc, argv, new Scene02);
    return 0;
}
