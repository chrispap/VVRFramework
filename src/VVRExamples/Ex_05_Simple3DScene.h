#include "settings.h"
#include "scene.h"
#include "mesh.h"
#include <vector>
#include <string>
#include <MathGeoLib/MathGeoLib.h>

class Simple3DScene : public vvr::Scene
{
public:
    Simple3DScene();
    const char* getName() const { return "Orientation Viewer";}
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void reset() override;

protected:
    void draw() override;

private:
    vvr::Settings   m_settings;
    vvr::Mesh       m_mesh;
    int             m_style_flag;
};
