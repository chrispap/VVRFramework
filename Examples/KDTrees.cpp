#include <vvr/scene.h>
#include <vvr/animation.h>
#include <vvr/kdtree.h>
#include <vvr/macros.h>

/**
* Array with 6 predefined colours.
*/
static const vvr::Colour Pallete[6] = {
    vvr::red, vvr::green, vvr::blue, vvr::magenta,
    vvr::orange, vvr::yellow,
};

class KDTreeScene : public vvr::Scene
{
    enum {
        BRUTEFORCE, POINTS_ON_SURFACE, SHOW_AXES, SHOW_FPS, SHOW_NN, SHOW_KNN, SHOW_SPHERE,
        SHOW_KDTREE, SHOW_PTS_ALL, SHOW_PTS_KDTREE,
        SHOW_PTS_IN_SPHERE,
    };

public:
    KDTreeScene();
    const char* getName() const { return "KD Tree Scene"; }
    void keyEvent(unsigned char key, bool up, int modif) override;
    void arrowEvent(vvr::ArrowDir dir, int modif) override;
    void mouseWheel(int dir, int modif) override;
    void sliderChanged(int slider_id, float val);

private:
    void draw() override;
    void reset() override;
    void resize() override;
    bool idle() override;
    void createRandomPts(int num_pts);
    void createSurfacePts(int num_pts);
    void printKeyboardShortcuts();

private:
    vvr::KDTree *m_KDTree;
    math::VecArray m_pts;
    vvr::Sphere3D::Ptr m_sphere;
    vvr::Animation m_anim;
    int m_flag;
    math::LCG m_lcg;
    int m_current_tree_level;
    float m_tree_invalidation_sec;
    int m_kn;
};

/**
* Function object to compare 2 3D-vecs in the specified axis.
*/
struct VecComparator {
    unsigned axis;
    VecComparator(unsigned axis) : axis(axis % 3) {}
    virtual inline bool operator() (const math::vec& v1, const math::vec& v2) {
        return (v1.ptr()[axis] < v2.ptr()[axis]);
    }
};

//! Task function prototypes

/**
* Find all the points under `root` node of the tree.
*/
void Task_01_FindPtsOfNode(const vvr::KDNode* root, math::VecArray &pts);

/**
* Find the nearest neighbour of `test_pt` inside `root`.
*/
void Task_02_Nearest(const math::vec& test_pt, const vvr::KDNode* root, const vvr::KDNode **nn, float *best_dist);

/**
* Find the points of `kdtree` that are contained inside `sphere`.
*/
void Task_03_InSphere(const math::Sphere &sphere, const vvr::KDNode *root, math::VecArray &pts);

/**
* Find the `k` nearest neighbours of `test_pt` inside `root`.
*/
void Task_04_NearestK(const int k, const math::vec& test_pt, const vvr::KDNode* root, const vvr::KDNode **knn, float *best_dist);

#define DIMENSIONS 3
#define NUM_PTS_DEFAULT 100
#define SEC_PER_FLOOR 10
#define GND_WIDTH 40
#define GND_TOP 10
#define GND_BOTTOM -10
#define GND_DEPTH 10
#define SPHERE_RAD 0.5
#define POINT_SIZE 6
#define AUTOPLAY false

//! KDTreeScene::

KDTreeScene::KDTreeScene()
{
    vvr::Shape::LineWidth = 1;
    vvr::Shape::PointSize = 10;
    m_bg_col = vvr::grey;
    m_perspective_proj = true;
    m_hide_log = false;
    m_hide_sliders = false;
    m_fullscreen = false;
    if (AUTOPLAY) m_anim.update(true);
    m_KDTree = NULL;
    reset();
}

void KDTreeScene::reset()
{
    Scene::reset();
    m_kn = 1;
    m_current_tree_level = 0;

    //! Position camera
    //auto pos = getFrustum().Pos();
    //pos.y += 20;
    //pos.z -= 40;
    //setCameraPos(pos);

    //! Define what will be vissible by default
    m_flag = 0;
    m_flag |= vvr_flag(SHOW_NN);
    m_flag |= vvr_flag(SHOW_PTS_KDTREE);
    m_flag |= vvr_flag(SHOW_KDTREE);
    m_flag |= vvr_flag(SHOW_PTS_ALL);
    m_flag |= vvr_flag(SHOW_PTS_IN_SPHERE);
    m_flag |= vvr_flag(BRUTEFORCE);

    //! Define scene objects
    m_sphere = vvr::Sphere3D::Make(-GND_WIDTH / 2, 0, 0, SPHERE_RAD, vvr::white);

    //! Create random points
    const float mw = getSceneWidth() * 0.3;
    const float mh = getSceneHeight() * 0.3;
    const float mz = std::min(mw, mh);

    if (vvr_flag_on(m_flag, POINTS_ON_SURFACE)) {
        createSurfacePts(m_pts.empty() ? NUM_PTS_DEFAULT : m_pts.size());
    }
    else {
        createRandomPts(m_pts.empty() ? NUM_PTS_DEFAULT : m_pts.size());
    }

    delete m_KDTree;
    m_KDTree = new vvr::KDTree(m_pts);
    m_tree_invalidation_sec = -1;

    //! Reset animation
    m_anim.setTime(0);
}

void KDTreeScene::resize()
{
    static bool first_pass = true;
    if (first_pass) printKeyboardShortcuts();
    first_pass = false;
}

void KDTreeScene::createRandomPts(int num_pts)
{
    m_pts.clear();
    for (int i = 0; i < num_pts; ++i) {
        float x = GND_WIDTH * 0.8 * (m_lcg.Float() - 0.5f);
        float y = GND_BOTTOM + (GND_TOP - GND_BOTTOM) * 0.8 * (m_lcg.Float() + 0.1);
        float z = GND_DEPTH * 0.8 * (m_lcg.Float() - 0.5f);
        m_pts.push_back(math::vec(x, y, z));
    }
    m_pts.shrink_to_fit();
}

void KDTreeScene::createSurfacePts(int num_pts)
{
    num_pts /= 10;
    num_pts += 10; // To avoid zero points which would cause crash.

    m_pts.clear();

    const int nx = math::Sqrt(num_pts) * GND_WIDTH / GND_DEPTH / 2;
    const int nz = num_pts / nx;

    const float x0 = GND_WIDTH * 0.8 * -0.5;
    const float x1 = GND_WIDTH * 0.8 * +0.5;
    const float dx = (x1 - x0) / nx;

    const float z0 = GND_DEPTH * 0.8 * -0.5f;
    const float z1 = GND_DEPTH * 0.8 * +0.5f;
    const float dz = (z1 - z0) / nz;

    const float y0 = (GND_TOP + GND_BOTTOM) * 0.5 - 1;
    const float y1 = (GND_TOP + GND_BOTTOM) * 0.5 + 1;

    for (size_t xi = 0; xi < nx; xi++)
    {
        for (size_t zi = 0; zi < nz; zi++)
        {
            float x = x0 + dx * xi;
            float z = z0 + dz * zi;
            float y = y0 + sin(x / 2) * sin(z / 2) * (y1 - y0);
            m_pts.push_back(math::vec(x, y, z));
        }
    }

    m_pts.shrink_to_fit();
}

bool KDTreeScene::idle()
{
    if (m_tree_invalidation_sec > 0 &&
        vvr::getSeconds() - m_tree_invalidation_sec > 0.8)
    {
        delete m_KDTree;
        m_KDTree = new vvr::KDTree(m_pts);
        m_tree_invalidation_sec = -1;
    }
    m_anim.update();
    return true;
}

void KDTreeScene::draw()
{
    const float POINT_SIZE_SAVE = vvr::Shape::PointSize;

    //! Draw axes
    if (m_flag & vvr_flag(SHOW_AXES)) drawAxes();

    //! Draw ground
    vvr::Ground ground(GND_WIDTH, GND_DEPTH, GND_BOTTOM, GND_TOP, vvr::Colour(35, 45, 55));
    ground.draw();

    //! Animate sphere
    float t = m_anim.t;
    vvr::Sphere3D sphere_moved(*m_sphere);
    sphere_moved.pos.x += t * ((float)GND_WIDTH / SEC_PER_FLOOR);
    math::vec sc(sphere_moved.pos);
    math::Sphere sphere(sc, sphere_moved.r);
    if (sphere_moved.pos.x > GND_WIDTH / 2) m_anim.setTime(0); // Bring back to start

    //! Draw points
    if (vvr_flag_on(m_flag, SHOW_PTS_ALL)) {
        for (size_t i = 0; i < m_pts.size(); i++) {
            vvr::Shape::PointSize = vvr::Shape::PointSize = POINT_SIZE;
            math2vvr(m_pts[i], vvr::white).draw();
            vvr::Shape::PointSize = POINT_SIZE_SAVE;
        }
    }

    //! Draw points in sphere
    if (vvr_flag_on(m_flag, SHOW_PTS_IN_SPHERE)) {
        if (vvr_flag_on(m_flag, SHOW_SPHERE)) {
            sphere_moved.draw();
        }
        math::VecArray pts_in;
        if (vvr_flag_on(m_flag, BRUTEFORCE)) {
            for (size_t i = 0; i < m_KDTree->pts.size(); i++)
                if (sphere.Contains(m_KDTree->pts.at(i))) pts_in.push_back(m_KDTree->pts.at(i));
        }
        else {
            Task_03_InSphere(sphere, m_KDTree->root(), pts_in);
        }
        vvr::Shape::PointSize = vvr::Shape::PointSize = POINT_SIZE;
        for (size_t i = 0; i < pts_in.size(); i++) {
            math2vvr(pts_in[i], vvr::magenta).draw();
        }
        vvr::Shape::PointSize = POINT_SIZE_SAVE;
    }

    //! Find and Draw Nearest Neighbour
    if (vvr_flag_on(m_flag, SHOW_NN)) {
        float dist;
        const vvr::KDNode *nearest = NULL;
        Task_02_Nearest(sc, m_KDTree->root(), &nearest, &dist);
        math::vec nn = nearest->split_point;
        vvr::Shape::PointSize = vvr::Shape::PointSize = POINT_SIZE;
        math2vvr(sc, vvr::blue).draw();
        math2vvr(nn, vvr::green).draw();
        vvr::Shape::PointSize = POINT_SIZE_SAVE;
    }

    //! Find and Draw K Nearest Neighbour
    if (vvr_flag_on(m_flag, SHOW_KNN)) {
        float dist;
        const vvr::KDNode **nearests = new const vvr::KDNode*[m_kn];
        memset(nearests, NULL, m_kn * sizeof(vvr::KDNode*));
        Task_04_NearestK(m_kn, sc, m_KDTree->root(), nearests, &dist);
        for (int i = 0; i < m_kn; i++) {
            if (!nearests[i]) continue;
            math::vec nn = nearests[i]->split_point;
            vvr::Shape::PointSize = vvr::Shape::PointSize = POINT_SIZE;
            math2vvr(sc, vvr::blue).draw();
            math2vvr(nn, vvr::green).draw();
            vvr::Shape::PointSize = POINT_SIZE_SAVE;
        }
        delete[] nearests;
    }

    //! Draw vvr::KDTree
    if (vvr_flag_on(m_flag, SHOW_KDTREE)) {
        for (int level = m_current_tree_level; level <= m_current_tree_level; level++) {
            std::vector<vvr::KDNode*> levelNodes = m_KDTree->getNodesOfLevel(level);
            for (int i = 0; i < levelNodes.size(); i++) {
                if (m_flag & vvr_flag(SHOW_PTS_KDTREE)) {
                    math::VecArray pts;
                    Task_01_FindPtsOfNode(levelNodes[i], pts);
                    vvr::Shape::PointSize = vvr::Shape::PointSize = POINT_SIZE;
                    for (int pi = 0; pi < pts.size(); pi++) {
                        math2vvr(pts[pi], Pallete[i % 6]).draw();
                    }
                    vvr::Shape::PointSize = POINT_SIZE_SAVE;
                }
                math::vec c1 = levelNodes[i]->aabb.minPoint;
                math::vec c2 = levelNodes[i]->aabb.maxPoint;
                vvr::Aabb3D box(c1.x, c1.y, c1.z, c2.x, c2.y, c2.z);
                box.setTransparency(0.9);
                box.colour = vvr::cyan;
                box.draw();
            }
        }
    }

    //! Compute & Display FPS
    static float last_update = 0;
    static float last_show = 0;
    const float sec = vvr::getSeconds();
    const float dt = sec - last_update;
    const float dt_show = sec - last_show;
    int FPS = 1.0 / dt;
    last_update = sec;
    if (vvr_flag_on(m_flag, SHOW_FPS) && dt_show >= 1) {
        vvr_echo(FPS);
        last_show = sec;
    }
}

void KDTreeScene::arrowEvent(vvr::ArrowDir dir, int modif)
{
    if (dir == vvr::UP)
    {
        ++m_current_tree_level;
        if (m_current_tree_level > m_KDTree->depth())
            m_current_tree_level = m_KDTree->depth();
    }
    else if (dir == vvr::DOWN)
    {
        --m_current_tree_level;
        if (m_current_tree_level < 0)
            m_current_tree_level = 0;
    }
}

void KDTreeScene::keyEvent(unsigned char key, bool up, int modif)
{
    Scene::keyEvent(key, up, modif);
    key = tolower(key);

    switch (key)
    {
        vvr_flag_toggle(m_flag, 'b', BRUTEFORCE);
        vvr_flag_toggle(m_flag, 'n', SHOW_NN);
        vvr_flag_toggle(m_flag, 'k', SHOW_KNN);
        vvr_flag_toggle(m_flag, 'f', SHOW_FPS);
        vvr_flag_toggle(m_flag, 'a', SHOW_AXES);
        vvr_flag_toggle(m_flag, 's', SHOW_SPHERE);
        vvr_flag_toggle(m_flag, 't', SHOW_KDTREE);
        vvr_flag_toggle(m_flag, 'p', SHOW_PTS_ALL);
        vvr_flag_toggle(m_flag, 'd', SHOW_PTS_KDTREE);
        vvr_flag_toggle(m_flag, 'c', SHOW_PTS_IN_SPHERE);
        vvr_flag_toggle(m_flag, 'u', POINTS_ON_SURFACE);
    }

    if (key == ' ')
    {
        if (m_anim.paused()) m_anim.update(true); else m_anim.pause();
    }
    else if (key == '?')
    {
        printKeyboardShortcuts();
    }
    else if (key == 'u')
    {
        if (vvr_flag_on(m_flag, POINTS_ON_SURFACE)) {
            createSurfacePts(m_pts.size());
        }
        else {
            createRandomPts(m_pts.size());
        }
        m_tree_invalidation_sec = vvr::getSeconds();
    }
}

void KDTreeScene::printKeyboardShortcuts()
{
    std::cout << "Keyboard shortcuts:"
        << std::endl << "'?' => This shortcut list:"
        << std::endl << "'b' => BRUTEFORCE"
        << std::endl << "'n' => SHOW_NN"
        << std::endl << "'k' => SHOW_KNN"
        << std::endl << "'f' => SHOW_FPS"
        << std::endl << "'a' => SHOW_AXES"
        << std::endl << "'s' => SHOW_SPHERE"
        << std::endl << "'t' => SHOW_KDTREE"
        << std::endl << "'p' => SHOW_PTS_ALL"
        << std::endl << "'d' => SHOW_PTS_KDTREE"
        << std::endl << "'c' => SHOW_PTS_IN_SPHERE"
        << std::endl << "'u' => POINTS_ON_SURFACE"
        << std::endl << std::endl;
}

void KDTreeScene::mouseWheel(int dir, int modif)
{
    if (false) // Placeholder
    {
    }
    else
    {
        Scene::mouseWheel(dir, modif);
    }
}

void KDTreeScene::sliderChanged(int slider_id, float v)
{
    switch (slider_id)
    {
    case 0:
        m_anim.setSpeed(v);
        break;
    case 1:
    {
        int num_pts = NUM_PTS_DEFAULT * (((100 * v)*(100 * v)) + 1);
        if (num_pts <= 3) num_pts = 3;
        if (vvr_flag_on(m_flag, POINTS_ON_SURFACE)) {
            createSurfacePts(num_pts);
        }
        else {
            createRandomPts(num_pts);
        }
        vvr_echo(m_pts.size());
    }
    m_tree_invalidation_sec = vvr::getSeconds();
    break;
    case 2:
        m_sphere->r = v * 30 * SPHERE_RAD;
        break;
    case 3:
        m_kn = v * 100;
        break;
    }
}

int main(int argc, char* argv[])
{
    try {
        return vvr::mainLoop(argc, argv, new KDTreeScene);
    }
    catch (std::string exc) {
        std::cerr << exc << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
        return 1;
    }
}

//! Tasks

void Task_01_FindPtsOfNode(const vvr::KDNode* root, math::VecArray &pts)
{
    pts.push_back(root->split_point);
    if (root->child_left) Task_01_FindPtsOfNode(root->child_left, pts);
    if (root->child_right) Task_01_FindPtsOfNode(root->child_right, pts);
}

void Task_02_Nearest(const math::vec& test_pt, const vvr::KDNode* root, const vvr::KDNode **nn, float *best_dist)
{
    if (!root) return;

    //! Distances
    const double d = test_pt.DistanceSq(root->split_point);
    const double d_split = root->split_point.ptr()[root->axis] - test_pt.ptr()[root->axis];
    const bool right_of_split = d_split <= 0;

    if (*nn == NULL || d < *best_dist) {
        *best_dist = d;
        *nn = root;
    }

    Task_02_Nearest(test_pt, right_of_split ? root->child_right : root->child_left, nn, best_dist);

    if (vvr_square(d_split) >= *best_dist) return;

    Task_02_Nearest(test_pt, right_of_split ? root->child_left : root->child_right, nn, best_dist);
}

void Task_03_InSphere(const math::Sphere &sphere, const vvr::KDNode *root, math::VecArray &pts)
{
    if (!root) return;

    //! Distances
    const double d = sphere.pos.DistanceSq(root->split_point);
    const double d_split = root->split_point.ptr()[root->axis] - sphere.pos.ptr()[root->axis];
    const bool right_of_split = d_split <= 0;

    if (d <= vvr_square(sphere.r)) pts.push_back(root->split_point);

    Task_03_InSphere(sphere, right_of_split ? root->child_right : root->child_left, pts);
    
    if (vvr_square(d_split) >= vvr_square(sphere.r)) return;

    Task_03_InSphere(sphere, right_of_split ? root->child_left : root->child_right, pts);
}

void Task_04_NearestK(const int k, const math::vec& test_pt, const vvr::KDNode* root, const vvr::KDNode **knn, float *best_dist)
{
    //...
    const vvr::KDNode *node = root;
    for (int i = 0; i < k; i++) {
        if (!node) return;
        knn[i] = node;
        node = node->child_right;
    }
}
