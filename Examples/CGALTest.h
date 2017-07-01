#include <vector>
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/random.hpp>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/algorithm.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/print_wavefront.h>
#include <CGAL/convex_hull_3.h>
#include <vvr/scene.h>
#include <vvr/logger.h>

using namespace boost::multiprecision;
using namespace boost::random;

// Define CGAL types
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Polyhedron_3<K> Polyhedron_3;
typedef K::Point_3 Point_3;

class CGALTestScene : public vvr::Scene
{
public:
    CGALTestScene();

private:
    std::vector<vvr::Point3D> m_pts;
    Polyhedron_3 m_ch_poly;
    bool m_pause;
    int m_style_flag;
    float m_time_last;
    uniform_01<mpf_float_50> uf;
    independent_bits_engine<mt19937, 50L * 1000L / 301L, mpz_int> gen;
    double m_inertia[6];

private:
    void keyEvent(unsigned char key, bool up, int modif) override;

    void resize() override;

    void reset() override;

    void draw() override;

    bool idle() override;

    void computeHull();
};
