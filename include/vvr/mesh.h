#ifndef VVR_MESH_H
#define VVR_MESH_H

#include "vvrframework_DLL.h"
#include "macros.h"
#include "utils.h"
#include "scene.h"
#include <MathGeoLib.h>
#include <vector>
#include <string>
#include <list>
#include <set>

namespace vvr {

/**
 * Enum used to control what to draw in a call of draw()
 */
enum VVRFramework_API Style {
    SOLID = (1 << 0),
    WIRE = (1 << 1),
    NORMALS = (1 << 2),
    BOUND = (1 << 3),
    AXES = (1 << 4),
};

struct VVRFramework_API Triangle
{
    /**
     * Indices to the veclist
     */
    union
    {
        struct { int vi1, vi2, vi3; };
        int v[3];
    };

    /**
     * Vector3fer to the std::vector containing the vertices
     */
    std::vector<math::vec> *vecList;

    /**
     * Plane equation coefficients
     */
    double A, B, C, D;

    Triangle(std::vector<math::vec> *vecList, int v1 = 0, int v2 = 0, int v3 = 0) :
        vi1(v1), vi2(v2), vi3(v3), vecList(vecList)
    {
        update();
    }

    /**
     * Calculate the coefficients of the plane from the vertices.
     */
    void update();

    const math::vec &v1() const;

    const math::vec &v2() const;

    const math::vec &v3() const;

    /**
     * Returns the normal of this triangle
     */
    const math::vec getNormal() const;

    /**
     * Returns the center Vector3f of this triangle
     */
    const math::vec getCenter() const;

    /**
     * Evaluates the plane equation of this triangle's plane
     * for the given Vector3f.
     * @param r The Vector3f at which we find the value of the plane equation.
     * @return The value of the plane equation at the given Vector3f.
     */
    double planeEquation(const math::vec &r) const;
};

/** 
 * Class that handles a 3D model.
 */
class VVRFramework_API Mesh
{
public:
    vvr_decl_shared_ptr(Mesh)
private:
    ~Mesh();

public:
    Mesh();
    Mesh(const std::string &objFile, const std::string &texFile=std::string(), bool ccw = true);
    Mesh(const Mesh &original);
    void operator=(const Mesh &src);
    void exportToObj(const std::string &filename);

private:
    std::vector<math::vec>  mVertices;              ///< Vertex list
    std::vector<Triangle>   mTriangles;             ///< Triangle list | contains indices to the Vertex list
    std::vector<math::vec>  mVertexNormals;         ///< Normals per vertex
    math::float3x4          mMatrix;                ///< Model rotation around its local axis
    math::AABB              mAABB;                  ///< The bounding box of the model
    bool                    mCCW;                   ///< Clockwise-ness

private:
    void updateTriangleData();                      ///< Recalculates the plane equations of the triangles
    void createNormals();                           ///< Create a normal for each vertex
    void drawTriangles(Colour col, bool wire = 0);  ///< Draw the triangles. This is the actual model drawing.
    void drawNormals(Colour col);                   ///< Draw the normals of each vertex
    void drawAxes();

public:
    void draw(Colour col, Style style);             ///< Draw the mesh with the specified style
    void move(const math::vec &p);                  ///< Move the mesh in the world.
    void setBigSize(float size);                    ///< Set the meshes size according to the max size of three (x|y|z)
    void cornerAlign();                             ///< Align the mesh to the corner of each local axis
    void centerAlign();                             ///< Align the mesh to the center of each local axis
    void update(const bool recomputeAABB=false);    ///< Call after making changes to the vertices
    void transform(const math::float3x4 &);         ///< Transforms the actual data.

    std::vector<math::vec> &getVertices() { return mVertices; }
    std::vector<Triangle> &getTriangles() { return mTriangles; }
    const std::vector<math::vec> &getVertices() const { return mVertices; }
    const std::vector<Triangle> &getTriangles() const { return mTriangles; }
    math::float3x4 getTransform() const { return mMatrix; }
    void setTransform(const math::float3x4 &t);
    math::AABB getAABB() const;
    float getMaxSize() const;
};

}

#endif 