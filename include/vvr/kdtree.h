#include "vvrframework_DLL.h"
#include <MathGeoLib.h>

namespace vvr {

    /**
     * A node of a KD-Tree
     */
    struct VVRFramework_API KDNode
    {
        KDNode() : child_left(NULL), child_right(NULL) {}
        ~KDNode() { delete child_left; delete child_right; }
        KDNode *child_left;
        KDNode *child_right;
        math::vec split_point;
        math::AABB aabb;
        int axis;
        int level;
    };

    /**
     * KD-Tree wrapper. Holds a ptr to tree root.
     */
    class VVRFramework_API KDTree
    {
    public:
        KDTree(math::VecArray &pts, int dimensions = 3);
        ~KDTree();
        std::vector<KDNode*> getNodesOfLevel(int level);
        int depth() const { return m_depth; }
        const KDNode* root() const { return m_root; }
        const math::VecArray &pts;

    private:
        int makeNode(KDNode *node, math::VecArray &pts, const int level);
        void getNodesOfLevel(KDNode *node, std::vector<KDNode*> &nodes, int level);

    private:
        KDNode *m_root;
        int m_DIM;
        int m_depth;
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

}
