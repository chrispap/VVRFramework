#include "settings.h"
#include "scene.h"
#include "mesh.h"

#include <vector>
#include <string>

using std::vector;
using std::string;

struct Bone {
    vvr::Mesh *mesh;
    vector<double> times;
    vector<vvr::Vec3d> rots;
};

class TemplateScene : public vvr::Scene
{
public: 
    TemplateScene();
    bool idle();
    const char* getName() const;

protected:
    void draw();
    void load();

private:
    void loadData(string, vector<vvr::Vec3d> &rots, vector<double> &times);

private:
    vvr::Settings m_settings;
    double m_bone_width, m_anim_start_time, tsf;
    bool m_anim_on;

private:
    Bone m_ulna, m_humerus;
};
