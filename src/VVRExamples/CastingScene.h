#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include "canvas.h"
#include "GeoLib.h"

/**
 * @brief The Simple2DScene class
 */
class CastingScene : public vvr::Scene
{
public: 
    CastingScene();
    const char* getName() const override;

protected:
    void draw() override;
    void reset() override;
    bool idle() override;
    void mousePressed(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;

private:
    C2DPoint               *m_curr_p;
    vvr::Canvas2D           m_canvas;
    std::vector<C2DPoint>   m_pts;
    C2DVector               m_displacement;
    C2DVector               m_dv;
    float                   m_last_update_time;
    bool                    m_anim_on;
};
