#include "scene.h"
#include "Renderable.h"

using namespace std;

/**
 * @brief The Simple2DScene class
 */
class PhysSimScene : public vvr::Scene
{
    vector<IRenderable*> m_renderables;
    float m_t;

public: 
    PhysSimScene();
    bool idle();
    const char* getName() const override;

protected:
    void draw() override;
};