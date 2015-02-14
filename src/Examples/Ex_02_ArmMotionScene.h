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

class ArmMotionScene : public vvr::Scene
{
public: 
    ArmMotionScene();
    bool idle();
    const char* getName() const;

protected:
    void draw();
    void load();

private:
    void loadData(string, vector<vvr::Vec3d> &rots, vector<double> &times);

private:
    vvr::Settings m_settings;
    vvr::Colour m_bone_col;
    double m_bone_width, m_anim_start_time, m_tsf;
    bool m_anim_on;
    Bone m_ulna, m_humerus;
};
