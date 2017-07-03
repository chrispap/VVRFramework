#ifndef SCENE_MOLDING_H
#define SCENE_MOLDING_H

#include <vvr/settings.h>
#include <vvr/scene.h>
#include <vvr/mesh.h>
#include <vvr/drawing.h>
#include "GeoLib.h"

class MoldingScene : public vvr::Scene
{
public:
    MoldingScene();

    const char* getName() const override {
        return "UNIVERSITY OF PATRAS - VVR GROUP - COMPUTATIONAL GEOMETRY LAB";
    }

protected:
    void draw() override;
    void reset() override;
    bool idle() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;

private:
    bool isFreeToMove(C2DVector &dx);

private:
    std::vector<C2DPoint> m_pts;
    vvr::Canvas2D m_canvas;
    C2DPoint *m_curr_p;
    C2DVector m_displacement;
    C2DVector m_dv;
    C2DPoint m_click_anchor;
    float m_last_update_time;
    bool m_anim_on;
};

#endif
