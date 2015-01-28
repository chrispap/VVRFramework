#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include "canvas.h"

class Simple2DScene : public vvr::Scene
{
public: 
    Simple2DScene();
    bool idle();
    const char* getName() const;

protected:
    void draw();
    void load();

private:
    vvr::Settings m_settings;

private:
    vvr::Canvas2D m_canvas;

};
