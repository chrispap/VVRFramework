#ifndef VVR_MESH_H
#define VVR_MESH_H

#include "vvrscenedll.h"
#include "geom.h"

#include <vector>
#include <string>
#include <list>
#include <set>

namespace vvr {

/**
* Enum used to controll what to draw in a call of draw()
*/
enum VVRScene_API Style {
    SOLID   = (1 << 0),
    WIRE    = (1 << 1),
    NORMALS = (1 << 2),
    BOUND   = (1 << 3),
    AXES    = (1 << 4),
};

/**
* Class that handles a 3D model.
*/
class VVRScene_API Mesh
{
public:
    Mesh ();
    ~Mesh ();
    Mesh (const std::string &objDir,
          const std::string &objFile,
          const std::string &texFile,
          bool ccw=true);
    Mesh (const Mesh *original);                    ///< Copy constructor
    Mesh (const Mesh &original);                    ///< Copy constructor
    void operator=(const Mesh *src);                ///< Assigment operator
    void operator=(const Mesh &src);                ///< Assigment operator

    VAR_CLASS_DEFS(Mesh)

private:
    // Data members
    bool                    mCCW;
    std::vector<Vec3d>      mVertices;              ///< Vertex list
    std::vector<Triangle>   mTriangles;             ///< Triangle list | contains indices to the Vertex list
    std::vector<Vec3d>      mVertexNormals;         ///< Normals per vertex
    std::vector<float>      mTexCoords;
    Box                     mAABB;                  ///< The bounding box of the model
    Vec3d                   mRot;                   ///< Model rotation around its local axis
    Vec3d                   mPos;                   ///< Model position in the scene
    unsigned                mTexName;

    void updateTriangleData ();                     ///< Recalculates the plane equations of the triangles
    void createNormals ();                          ///< Create a normal for each vertex
    void drawTriangles (Colour col,bool wire=0);    ///< Draw the triangles. This is the actual model drawing.
    void drawNormals (Colour col);                  ///< Draw the normals of each vertex
    void drawAxes ();

public:
    void draw (Colour col, Style style);            ///< Draw the mesh with the specified style
    void move   (const Vec3d &p);                   ///< Move the mesh in the world.
    void rotate (const Vec3d &p);                   ///< Rotate mesh around its local axis
    void setPos (const Vec3d &p) { mPos = p;}       ///< Set the position of the mesh
    void setRot (const Vec3d &p) { mRot = p;}       ///< Set the rotation of the mesh
    void setBigSize (float size);                   ///< Set the meshes size according to the max size of three (x|y|z)
    void cornerAlign ();                            ///< Align the mesh to the corner of each local axis
    void centerAlign ();                            ///< Align the mesh to the center of each local axis

    Vec3d getPos() const {return mPos;}
    Vec3d getRot() const {return mRot;}
    Box   getBox() const {return mAABB;}

    void update();                                  ///< Call after making changes to the vertices
    std::vector<Vec3d> &getVertices() { return mVertices; }
    std::vector<Triangle> &getTriangles() { return mTriangles; }
    const std::vector<Vec3d> &getVertices() const { return mVertices; }
    const std::vector<Triangle> &getTriangles() const { return mTriangles; }
};

}

#endif // VVR_MESH_H
