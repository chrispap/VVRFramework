#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include "canvas.h"

/**
 * @brief The Simple2DScene class
 */
class Simple2DScene : public vvr::Scene
{
public: 
    Simple2DScene();
    const char* getName() const;

protected:
    void draw();
    void load();
    void mousePressed(int x, int y, int modif);
    void mouseMoved(int x, int y, int modif);
    void mouseWheel(int dir, int modif);
    void reset();

private:
    void mouse2pix(int &x, int &y);

private:
    vvr::Settings m_settings;
    vvr::Canvas2D m_canvas;

    float m_rad;
};
