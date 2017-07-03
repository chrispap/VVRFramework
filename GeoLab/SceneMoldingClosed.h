#ifndef SCENE_MOLDING_H
#define SCENE_MOLDING_H

#include <vvr/scene.h>
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
    void findMoldPieces();

private:
    C2DPoint *m_curr_p, m_click_anchor;
    C2DPolygon m_polygon;
    std::vector<C2DPoint> m_pts;
    vvr::Animation m_anim;
    std::vector<C2DPolygon> m_pieces;
    std::vector<C2DVector> m_dirs;
};

#endif
