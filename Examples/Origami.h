#include <vvr/scene.h>
#include <vvr/utils.h>
#include <vvr/canvas.h>
#include <MathGeoLib.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
#include <set>

struct Fold
{
    math::Polygon pol;
    vvr::Colour col;
    float4x4 M = float4x4::identity;
};

struct Animation
{
    bool on;
    float time;
    float last_update;
    float speed = 1;
};

class OrigamiScene : public vvr::Scene
{
public:
    OrigamiScene();
    const char* getName() const { return "Origami Scene"; }
    void mousePressed(int x, int y, int modif) override;
    void mouseMoved(int x, int y, int modif) override;
    void mouseReleased(int x, int y, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void sliderChanged(int slider_id, float val) override;

protected:
    void draw() override;
    void reset() override;
    bool idle() override;

private:
    void makePaper();
    void newLineSegment(int x, int y, bool shiftDown);
    void smoothSlices();
    void triangulate();
    void undo();
    
private:
    vvr::Canvas2D m_canvas;
    std::vector<Fold> folds;
    math::vec A, B, C, D, E, F;
    C2DPolygon m_polygon;
    C2DPolygonSet m_polygon_set;
    Animation anim;
    unsigned m_ongoing_slicing_count;
    int m_style_flag;
};
