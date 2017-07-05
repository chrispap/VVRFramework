#ifndef SCENE_AREA_H
#define SCENE_AREA_H

#include <vvr/scene.h>

class SceneArea : public vvr::Scene
{
public:
    SceneArea();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected:
    void draw() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;

private:
    float m_rad;
    int m_x;
    int m_y;
};

#endif // SCENE_AREA_H
#include <vvr/macros.h>

using namespace vvr;
using namespace std;

SceneArea::SceneArea()
{
    m_bg_col = Colour::grey;
    m_rad = 30;
    m_x = 300;
    m_y = 0;
    m_hide_log = false;
    m_hide_sliders = false;
}

void SceneArea::draw()
{
    // Instruct VVRFramework to render in pixel coordinates.
    enterPixelMode();

    Colour col_circ = Colour::black;
    Colour col_tri = Colour::black;

    // Compute areas.
    // Triangle area: http://en.wikipedia.org/wiki/Triangle#Using_coordinates

    double area_circ = 3.14159 * m_rad * m_rad;
    double area_tri = 0.5 * abs((-200.0 - m_x) * (200.0 - (-200.0)) -
        (-200.0 - (-200.0)) * (m_y - (-200.0)));

    vvr_echo(area_circ);
    vvr_echo(area_tri);

    if (area_circ > area_tri) {
        col_circ = Colour::green;
    }
    else {
        col_tri = Colour::green;
    }

    // Draw a circle centered at: (m_x , m_y).
    Circle2D circle = Circle2D(m_x, m_y, m_rad, col_circ);
    circle.draw();

    // Draw a triangle.
    Triangle2D tri(-200, -200, -200, 200, m_x, m_y, col_tri);
    tri.setSolidRender(false);
    tri.draw();

    // Draw a line segment.
    LineSeg2D line(300, -200, m_x, m_y, Colour::white);
    line.draw();
}

void SceneArea::mousePressed(int x, int y, int modif)
{
    m_x = x;
    m_y = y;
}

void SceneArea::mouseMoved(int x, int y, int modif)
{
    m_x = x;
    m_y = y;
}

void SceneArea::mouseWheel(int dir, int modif)
{
    if (dir > 0)
    {
        m_rad += 9;
    }
    else
    {
        m_rad -= 9;
        if (m_rad < 0) m_rad = 0;
    }
}

/* Application entry point */

int main(int argc, char* argv[])
{
    return vvr::mainLoop(argc, argv, new SceneArea());
}
